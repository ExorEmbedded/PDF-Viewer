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

#ifndef TOC_H
#define TOC_H

#include "abstractinfodock.h"
#include "flickcharm.h"

#include <poppler-qt4.h>

#include <QTreeWidgetItem>

class QTreeWidget;

class TocDock : public AbstractInfoDock
{
    Q_OBJECT

public:
    TocDock(QWidget *parent = 0);
    ~TocDock();

    /*virtual*/ void documentClosed();
    /*virtual*/ void documentLoaded();

protected:
    /*virtual*/ void fillInfo();
    /*virtual*/ void fillToc(const QDomNode &parent, QTreeWidget *tree, QTreeWidgetItem *parentItem);

private Q_SLOTS:
    void toggleTOC();
    void itemClick(QTreeWidgetItem* qmi, int col);
    void itemDoubleClick(QTreeWidgetItem* qmi, int col);

private:
    QTreeWidget *m_tree;
    FlickCharm* flickCharm;
};

#endif