/*
 * Copyright (C) 2008-2009, Pino Toscano <pino@kde.org>
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

#include "navigationtoolbar.h"

#include <poppler-qt4.h>
#include "viewer.h"

#include <QtGui/QAction>
#include <QtGui/QComboBox>
#include <QtGui/QLayout>
#include <QDebug>
#include <QDialog>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QScrollArea>
#include <QPushButton>

NavigationToolBar::NavigationToolBar(PdfViewer *parent)
    : QToolBar((QWidget*)parent)
{
    flickCharm = new FlickCharm(this);

    setFloatable(false);
    setMovable(false);
	m_tocAct = addAction(QIcon(QPixmap(":/icons/toc.png")), tr("TOC"), parent->getTocDock(), SLOT(toggleTOC()));
    m_tocAct->setCheckable(true);
    widgetForAction(m_tocAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	//m_firstAct = addAction(tr("First"), this, SLOT(slotGoFirst()));
	//widgetForAction(m_firstAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	m_prevAct = addAction(QIcon(QPixmap(":/icons/arrowL.png")), tr("Previous Page"), this, SLOT(slotGoPrev()));
    widgetForAction(m_prevAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_pageCombo = new QComboBox(this);
    QAbstractItemView* pageView = m_pageCombo->view();
    pageView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    flickCharm->activateOn(pageView);
    m_pageCombo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    connect(m_pageCombo, SIGNAL(activated(int)), this, SLOT(slotComboActivated(int)));
    addWidget(m_pageCombo);

	m_nextAct = addAction(QIcon(QPixmap(":/icons/arrowR.png")), tr("Next Page"), this, SLOT(slotGoNext()));
    widgetForAction(m_nextAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    //m_lastAct = addAction(tr("Last"), this, SLOT(slotGoLast()));
    //widgetForAction(m_lastAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    addSeparator();

    m_zoomCombo = new QComboBox(this);
    QAbstractItemView* zoomView = m_zoomCombo->view();
    zoomView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    flickCharm->activateOn(zoomView);
    m_zoomCombo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    //m_zoomCombo->setEditable(true);
    m_zoomCombo->addItem(tr("10%"));
    m_zoomCombo->addItem(tr("25%"));
    m_zoomCombo->addItem(tr("33%"));
    m_zoomCombo->addItem(tr("50%"));
    m_zoomCombo->addItem(tr("66%"));
    m_zoomCombo->addItem(tr("75%"));
    m_zoomCombo->addItem(tr("100%"));
    m_zoomCombo->addItem(tr("125%"));
    m_zoomCombo->addItem(tr("150%"));
    m_zoomCombo->addItem(tr("200%"));
    m_zoomCombo->addItem(tr("300%"));
    m_zoomCombo->addItem(tr("400%"));
    m_zoomCombo->setCurrentIndex(6); // "100%"
    connect(m_zoomCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotZoomComboChanged(QString)));
    addWidget(m_zoomCombo);

	m_fitAct = addAction(QIcon(QPixmap(":/icons/fit.png")), tr("Fit to Size"), this, SLOT(slotFit()));
	widgetForAction(m_fitAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	m_fitAct->setCheckable(true);

    m_rotationCombo = new QComboBox(this);
    QAbstractItemView* rotationView = m_rotationCombo->view();
    rotationView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    flickCharm->activateOn(rotationView);
    m_rotationCombo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    //m_rotationCombo->setA
    // NOTE: \302\260 = degree symbol
    m_rotationCombo->addItem(trUtf8("0\302\260"));
    m_rotationCombo->addItem(trUtf8("90\302\260"));
    m_rotationCombo->addItem(trUtf8("180\302\260"));
    m_rotationCombo->addItem(trUtf8("270\302\260"));
    connect(m_rotationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRotationComboChanged(int)));
    addWidget(m_rotationCombo);

	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	addWidget(spacer);
	m_openAct = addAction(QIcon(QPixmap(":/icons/open.png")), tr("Open pdf"), parent, SLOT(slotOpenFile()));
	widgetForAction(m_openAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	QDialog* dialog = new QDialog(this);
	dialog->setFixedSize(320,240);
	dialog->setWindowTitle("License");
	dialog->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	QTextEdit* te= new QTextEdit();
	te->setReadOnly(true);
	te->setText(GPL3_LICENSE);
	flickCharm->activateOn(te);
	QPushButton* okButton = new QPushButton("OK", dialog);
	okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
	QVBoxLayout* vbl = new QVBoxLayout();
	vbl->addWidget(te);
	vbl->addWidget(okButton);
	dialog->setLayout(vbl);
	vbl->setAlignment(okButton, Qt::AlignRight);
	m_aboutAct = addAction(QIcon(QPixmap(":/icons/license.png")), tr("About"), dialog, SLOT(open()));
	widgetForAction(m_aboutAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	m_closeAct = addAction(QIcon(QPixmap(":/icons/close.png")), tr("Exit"), qApp, SLOT(closeAllWindows()));
	widgetForAction(m_closeAct)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    documentClosed();
}

NavigationToolBar::~NavigationToolBar()
{
}

void NavigationToolBar::documentLoaded()
{
    const int pageCount = document()->numPages();
    for (int i = 0; i < pageCount; ++i) {
        m_pageCombo->addItem(QString::number(i + 1));
    }
    m_pageCombo->setEnabled(true);
}

void NavigationToolBar::documentClosed()
{
    //m_firstAct->setEnabled(false);
    m_prevAct->setEnabled(false);
    m_nextAct->setEnabled(false);
    //m_lastAct->setEnabled(false);
    m_pageCombo->clear();
    m_pageCombo->setEnabled(false);
}

void NavigationToolBar::pageChanged(int page)
{
    const int pageCount = document()->numPages();
    //m_firstAct->setEnabled(page > 0);
    m_prevAct->setEnabled(page > 0);
    m_nextAct->setEnabled(page < (pageCount - 1));
    //m_lastAct->setEnabled(page < (pageCount - 1));
    m_pageCombo->setCurrentIndex(page);
}

void NavigationToolBar::slotGoFirst()
{
    setPage(0);
}

void NavigationToolBar::slotGoPrev()
{
    setPage(page() - 1);
}

void NavigationToolBar::slotGoNext()
{
    setPage(page() + 1);
}

void NavigationToolBar::slotGoLast()
{
    setPage(document()->numPages() - 1);
}

void NavigationToolBar::slotComboActivated(int index)
{
    setPage(index);
}

void NavigationToolBar::slotZoomComboChanged(const QString &_text)
{
    QString text = _text;
    text.remove(QLatin1Char('%'));
    bool ok = false;
    int value = text.toInt(&ok);
    if (ok && value >= 10) {
        emit zoomChanged(qreal(value) / 100);
    }
}

void NavigationToolBar::slotRotationComboChanged(int idx)
{
    emit rotationChanged(idx * 90);
}

void NavigationToolBar::slotFit()
{
	if(m_fitAct->isChecked())
	{
		m_zoomCombo->setEnabled(false);
		emit zoomChanged(0);
	} else
	{
		m_zoomCombo->setEnabled(true);
		slotZoomComboChanged(m_zoomCombo->currentText());
	}
}
