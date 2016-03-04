/*
 * Copyright (C) 2008, Pino Toscano <pino@kde.org>
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

#include "toc.h"

//#include <poppler-qt4.h>
#include <poppler-link.h>

#include <QtGui/QHeaderView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QAction>
#include <QDebug>

void TocDock::fillToc(const QDomNode &parent, QTreeWidget *tree, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *newitem = 0;
    for (QDomNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling()) {
        QDomElement e = node.toElement();

        if (!parentItem) {
            newitem = new QTreeWidgetItem(tree, newitem);
        } else {
            newitem = new QTreeWidgetItem(parentItem, newitem);
        }
        newitem->setText(0, e.tagName());

		QString dest = e.attribute("Destination");

        if(!dest.isEmpty())
        {
			Poppler::LinkDestination ld = Poppler::LinkDestination(dest);
			newitem->setData(0, Qt::UserRole, QVariant(ld.pageNumber()-1));
		} else
		{
			dest = e.attribute("DestinationName");
			if (!dest.isEmpty())
			{
				Poppler::LinkDestination* ld = document()->linkDestination(dest);
				newitem->setData(0, Qt::UserRole, QVariant(ld->pageNumber()-1));
			}
			else
				newitem->setData(0, Qt::UserRole, QVariant(-1));
		}

        bool isOpen = false;
        if (e.hasAttribute(QString::fromLatin1("Open"))) {
            isOpen = QVariant(e.attribute(QString::fromLatin1("Open"))).toBool();
        }
        if (isOpen) {
            tree->expandItem(newitem);
        }
    }
}


TocDock::TocDock(QWidget *parent)
    : AbstractInfoDock(parent)
{

    flickCharm = new FlickCharm(this);

    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setTitleBarWidget(new QWidget());
	setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);

	/*QWidget *mainWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSizeConstraint(QLayout::SetNoConstraint);
    layout->setAlignment(Qt::AlignHCenter);
    layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

    mainWidget->setLayout(layout);
	mainWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);*/

    m_tree = new QTreeWidget(this);
	m_tree->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    m_tree->setAlternatingRowColors(true);
	m_tree->header()->hide();
    m_tree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_tree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

	QFont treeFont = QFont();
	treeFont.setPixelSize(13);
	m_tree->setFont(treeFont);

    flickCharm->activateOn(m_tree);

    connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemClick(QTreeWidgetItem*, int)));
    connect(m_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemDoubleClick(QTreeWidgetItem*, int)));

	/*QPushButton* TOCButton = new QPushButton("TOC");
    TOCButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
    TOCButton->setFlat(true);
    TOCButton->setFocusPolicy(Qt::NoFocus);
    TOCButton->setStyleSheet("padding: 6px; margin: 0px; border: none; background-color: #D0D8E1");
	connect(TOCButton, SIGNAL (clicked()),this, SLOT (toggleTOC()));*/

	//layout->addWidget(m_tree);
	//layout->addWidget(TOCButton);

	setWidget(m_tree);
	hide();
}

TocDock::~TocDock()
{
}

void TocDock::fillInfo()
{
    const QDomDocument *toc = document()->toc();
    if (toc) {
        fillToc(*toc, m_tree, 0);
    } else {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, tr("No TOC"));
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        m_tree->addTopLevelItem(item);
    }
	m_tree->resizeColumnToContents(0);
}

void TocDock::documentClosed()
{
    m_tree->clear();
    AbstractInfoDock::documentClosed();
}

void TocDock::documentLoaded()
{
    fillInfo();
    m_filled = true;
}

void TocDock::toggleTOC()
{
	if(isVisible())
		hide();
    else
		show();
}

void TocDock::itemClick(QTreeWidgetItem* qmi, int col)
{
    qmi->setExpanded(!qmi->isExpanded());
	m_tree->resizeColumnToContents(col);

    int page = qmi->data(0, Qt::UserRole).toInt();
	if(page >= 0)
        setPage(page);
}

void TocDock::itemDoubleClick(QTreeWidgetItem* qmi, int col)
{

}
