/*
 * Copyright (C) 2008-2009, Pino Toscano <pino@kde.org>
 * Copyright (C) 2008, Albert Astals Cid <aacid@kde.org>
 * Copyright (C) 2009, Shawn Rutledge <shawn.t.rutledge@gmail.com>
 * Copyright (C) 2013, Fabio D'Urso <fabiodurso@hotmail.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "viewer.h"

#include "embeddedfiles.h"
#include "fonts.h"
#include "info.h"
#include "metadata.h"
#include "navigationtoolbar.h"
#include "optcontent.h"
#include "pageview.h"
#include "permissions.h"
#include "thumbnails.h"

#include "flickcharm.h"

#include <poppler-qt4.h>

#include <QtCore/QDir>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>

PdfViewer::PdfViewer()
	: QMainWindow(), m_currentPage(0), m_doc(0), m_pdfPath("")
{

	// Remove QMainWindow separator
	setStyleSheet("QMainWindow::separator { width: 0px; height: 0px; }"
				  "QMainWindow { border: 0px; margin: 0px; padding: 0px;}"
				  "QToolButton { padding: 1px; }"
				  "QToolBar { spacing: 3px;  margin: 2px; }"
				  "QTreeWidgetItem { font-size: 15px; margin: 5px; spacing: 5px; padding: 5px; }"
				  "QLineEdit { margin: 5px; spacing: 5px; padding: 5px; }"
				  "QScrollArea { border: 0px; margin: 0px; padding: 0px; }");

	QDBusConnection connection = QDBusConnection::sessionBus();
	if (connection.isConnected())
		fileBrowser = new com::exor::FileBrowser("com.exor.FileBrowser", "/", connection, this);

	evlp = new QEventLoop(this);
	connect(this, SIGNAL(wakeUp()), evlp, SLOT(quit()));
	connect(fileBrowser,SIGNAL(done(QString)), this, SLOT(retPath(QString)));

	QAction *act;

    // Setup the menus
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileOpenAct = fileMenu->addAction(tr("&Open"), this, SLOT(slotOpenFile()));
    m_fileOpenAct->setShortcut(Qt::CTRL + Qt::Key_O);
    fileMenu->addSeparator();
    m_fileSaveCopyAct = fileMenu->addAction(tr("&Save a Copy..."), this, SLOT(slotSaveCopy()));
    m_fileSaveCopyAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    m_fileSaveCopyAct->setEnabled(false);
    fileMenu->addSeparator();
	act = fileMenu->addAction(tr("&Quit"), qApp, SLOT(closeAllWindows()));
    act->setShortcut(Qt::CTRL + Qt::Key_Q);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));
    m_settingsTextAAAct = settingsMenu->addAction(tr("Text Antialias"));
    m_settingsTextAAAct->setCheckable(true);
    connect(m_settingsTextAAAct, SIGNAL(toggled(bool)), this, SLOT(slotToggleTextAA(bool)));
    m_settingsGfxAAAct = settingsMenu->addAction(tr("Graphics Antialias"));
    m_settingsGfxAAAct->setCheckable(true);
    connect(m_settingsGfxAAAct, SIGNAL(toggled(bool)), this, SLOT(slotToggleGfxAA(bool)));
    QMenu *settingsRenderMenu = settingsMenu->addMenu(tr("Render Backend"));
    m_settingsRenderBackendGrp = new QActionGroup(settingsRenderMenu);
    m_settingsRenderBackendGrp->setExclusive(true);
    act = settingsRenderMenu->addAction(tr("Splash"));
    act->setCheckable(true);
    act->setChecked(true);
    act->setData(qVariantFromValue(0));
    m_settingsRenderBackendGrp->addAction(act);
	act = settingsRenderMenu->addAction(tr("Author"));
    act->setCheckable(true);
    act->setData(qVariantFromValue(1));
    m_settingsRenderBackendGrp->addAction(act);
    connect(m_settingsRenderBackendGrp, SIGNAL(triggered(QAction*)),
            this, SLOT(slotRenderBackend(QAction*)));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    act = helpMenu->addAction(tr("&About"), this, SLOT(slotAbout()));
    act = helpMenu->addAction(tr("About &Qt"), this, SLOT(slotAboutQt()));

	menuBar()->setVisible(false);

    FlickCharm* flickCharm = new FlickCharm(this);

    PageView *view = new PageView(this);
    setCentralWidget(view);
    flickCharm->activateOn(view);
    m_observers.append(view);

	/*InfoDock *infoDock = new InfoDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, infoDock);
    infoDock->hide();
    viewMenu->addAction(infoDock->toggleViewAction());
	m_observers.append(infoDock);*/

    tocDock = new TocDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, tocDock);
	viewMenu->addAction(tocDock->toggleViewAction());
    m_observers.append(tocDock);

   /* FontsDock *fontsDock = new FontsDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, fontsDock);
    fontsDock->hide();
    viewMenu->addAction(fontsDock->toggleViewAction());
    m_observers.append(fontsDock);

    PermissionsDock *permissionsDock = new PermissionsDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, permissionsDock);
    permissionsDock->hide();
    viewMenu->addAction(permissionsDock->toggleViewAction());
    m_observers.append(permissionsDock);

    ThumbnailsDock *thumbnailsDock = new ThumbnailsDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, thumbnailsDock);
    thumbnailsDock->hide();
    viewMenu->addAction(thumbnailsDock->toggleViewAction());
    m_observers.append(thumbnailsDock);

    EmbeddedFilesDock *embfilesDock = new EmbeddedFilesDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, embfilesDock);
    embfilesDock->hide();
    viewMenu->addAction(embfilesDock->toggleViewAction());
    m_observers.append(embfilesDock);

    MetadataDock *metadataDock = new MetadataDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, metadataDock);
    metadataDock->hide();
    viewMenu->addAction(metadataDock->toggleViewAction());
    m_observers.append(metadataDock);

    OptContentDock *optContentDock = new OptContentDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, optContentDock);
    optContentDock->hide();
    viewMenu->addAction(optContentDock->toggleViewAction());
	m_observers.append(optContentDock);*/

    NavigationToolBar *navbar = new NavigationToolBar(this);
    addToolBar(navbar);
    m_observers.append(navbar);

/*    QToolBar *rightbar = new QToolBar(this);
    rightbar->setFloatable(false);
    rightbar->setMovable(false);
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightbar->addWidget(spacer);
	act = rightbar->addAction(QIcon(QPixmap("icons/open.png")), tr("Open pdf"), this, SLOT(slotOpenFile()));
    rightbar->widgetForAction(act)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	act = rightbar->addAction(QIcon(QPixmap("icons/close.png")), tr("Exit"), qApp, SLOT(closeAllWindows()));
    rightbar->widgetForAction(act)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	addToolBar(rightbar);*/

    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->m_viewer = this;
    }

    connect(navbar, SIGNAL(zoomChanged(qreal)), view, SLOT(slotZoomChanged(qreal)));
    connect(navbar, SIGNAL(rotationChanged(int)), view, SLOT(slotRotationChanged(int)));

	// Enable text AA
	m_settingsTextAAAct->setChecked(true);
	m_settingsGfxAAAct->setChecked(false);
}

PdfViewer::~PdfViewer()
{
    closeDocument();
}

QSize PdfViewer::sizeHint() const
{
    return QSize(500, 600);
}

void PdfViewer::loadDocument(const QString &file)
{
    Poppler::Document *newdoc = Poppler::Document::load(file);
    if (!newdoc) {
        QMessageBox msgbox(QMessageBox::Critical, tr("Open Error"), tr("Cannot open:\n") + file,
                           QMessageBox::Ok, this);
        msgbox.exec();
        return;
    }

    while (newdoc->isLocked()) {
        bool ok = true;
        QString password = QInputDialog::getText(this, tr("Document Password"),
                                                 tr("Please insert the password of the document:"),
                                                 QLineEdit::Password, QString(), &ok);
        if (!ok) {
            delete newdoc;
            return;
        }
        newdoc->unlock(password.toLatin1(), password.toLatin1());
    }

    closeDocument();

    m_doc = newdoc;

    m_doc->setRenderHint(Poppler::Document::TextAntialiasing, m_settingsTextAAAct->isChecked());
    m_doc->setRenderHint(Poppler::Document::Antialiasing, m_settingsGfxAAAct->isChecked());
    m_doc->setRenderBackend((Poppler::Document::RenderBackend)m_settingsRenderBackendGrp->checkedAction()->data().toInt());

    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->documentLoaded();
        obs->pageChanged(0);
    }

    setWindowTitle(file);

	m_fileSaveCopyAct->setEnabled(true);
}

void PdfViewer::closeDocument()
{
    if (!m_doc) {
        return;
    }

    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->documentClosed();
    }

    m_currentPage = 0;
    delete m_doc;
    m_doc = 0;

    setWindowTitle("PDF Viewer");

    m_fileSaveCopyAct->setEnabled(false);
}

void PdfViewer::slotOpenFile()
{

	QString style = QString(
		"QWidget#scrollAreaWidgetContents { border: 1px solid #7F8084; } \
		QDialog { border:none; } \
		QPushButton#okButton, QPushButton#cancelButton { height: 18px; padding: 3px; } \
		QPushButton, QLabel { font-size: 12px; } \
		LabelButton { background: #0080FF; border: 1px solid #000000; padding: 5px; margin: 3px; } ");

	QDBusPendingReply<int> reply = fileBrowser->getOpenFileName("/", QStringList(), QStringList() << "*.pdf",
													   "Open PDF Document:", QStringList(), style);
	reply.waitForFinished();

	if (!reply.isError() && reply.argumentAt<0>() == 0)
		evlp->exec();
	 else
		m_pdfPath = QFileDialog::getOpenFileName(this, tr("Open PDF Document"), QDir::rootPath(), tr("PDF Documents (*.pdf)"));

	if (m_pdfPath.isEmpty()) {
        return;
    }

	loadDocument(m_pdfPath);
	m_pdfPath="";
}

void PdfViewer::slotSaveCopy()
{
    if (!m_doc) {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Copy"), QDir::homePath(), tr("PDF Documents (*.pdf)"));
    if (fileName.isEmpty()) {
        return;
    }

    Poppler::PDFConverter *converter = m_doc->pdfConverter();
    converter->setOutputFileName(fileName);
    converter->setPDFOptions(converter->pdfOptions() & ~Poppler::PDFConverter::WithChanges);
    if (!converter->convert()) {
        QMessageBox msgbox(QMessageBox::Critical, tr("Save Error"), tr("Cannot export to:\n%1").arg(fileName),
                           QMessageBox::Ok, this);
    }
    delete converter;
}

void PdfViewer::slotAbout()
{
    const QString text("This is a demo of the Poppler-Qt4 library.");
    QMessageBox::about(this, QString::fromLatin1("About Poppler-Qt4 Demo"), text);
}

void PdfViewer::slotAboutQt()
{
    QMessageBox::aboutQt(this);
}

void PdfViewer::slotToggleTextAA(bool value)
{
    if (!m_doc) {
        return;
    }

    m_doc->setRenderHint(Poppler::Document::TextAntialiasing, value);

    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->pageChanged(m_currentPage);
    }
}

void PdfViewer::slotToggleGfxAA(bool value)
{
    if (!m_doc) {
        return;
    }

    m_doc->setRenderHint(Poppler::Document::Antialiasing, value);

    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->pageChanged(m_currentPage);
    }
}

void PdfViewer::slotRenderBackend(QAction *act)
{
    if (!m_doc || !act) {
        return;
    }

    m_doc->setRenderBackend((Poppler::Document::RenderBackend)act->data().toInt());

    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->pageChanged(m_currentPage);
    }
}

void PdfViewer::setPage(int page)
{
    Q_FOREACH(DocumentObserver *obs, m_observers) {
        obs->pageChanged(page);
    }

    m_currentPage = page;
}

int PdfViewer::page() const
{
    return m_currentPage;
}

TocDock* PdfViewer::getTocDock(){
    return tocDock;
}
