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

#ifndef NAVIGATIONTOOLBAR_H
#define NAVIGATIONTOOLBAR_H

#include <QtGui/QToolBar>
#include <QString>
#include "flickcharm.h"
#include "documentobserver.h"

class QAction;
class QComboBox;

class NavigationToolBar : public QToolBar, public DocumentObserver
{
    Q_OBJECT

public:
    NavigationToolBar(PdfViewer *parent = 0);
    ~NavigationToolBar();

    /*virtual*/ void documentLoaded();
    /*virtual*/ void documentClosed();
    /*virtual*/ void pageChanged(int page);

Q_SIGNALS:
    void zoomChanged(qreal value);
    void rotationChanged(int rotation);

private Q_SLOTS:
    void slotGoFirst();
    void slotGoPrev();
    void slotGoNext();
    void slotGoLast();
    void slotComboActivated(int index);
    void slotZoomComboChanged(const QString &text);
    void slotRotationComboChanged(int idx);
    void slotFit();

private:
    //QAction *m_firstAct;
    QAction *m_prevAct;
    QAction *m_tocAct;
    QComboBox *m_pageCombo;
    QAction *m_nextAct;
    QAction *m_fitAct;
	QAction *m_openAct;
	QAction *m_aboutAct;
	QAction *m_closeAct;
    //QAction *m_lastAct;
    QComboBox *m_zoomCombo;
    QComboBox *m_rotationCombo;

    FlickCharm* flickCharm;
};

const QString GPL3_LICENSE = "GNU GENERAL PUBLIC LICENSE\n\
Version 3, 29 June 2007\n\
\n\
Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>\
Everyone is permitted to copy and distribute verbatim copies\
of this license document, but changing it is not allowed.\n\
\n\
Preamble\n\
\n\
The GNU General Public License is a free, copyleft license for\
software and other kinds of works.\n\
\n\
The licenses for most software and other practical works are designed\
to take away your freedom to share and change the works.  By contrast,\
the GNU General Public License is intended to guarantee your freedom to\
share and change all versions of a program--to make sure it remains free\
software for all its users.  We, the Free Software Foundation, use the\
GNU General Public License for most of our software; it applies also to\
any other work released this way by its authors.  You can apply it to\
your programs, too.\n\
\n\
When we speak of free software, we are referring to freedom, not\
price.  Our General Public Licenses are designed to make sure that you\
have the freedom to distribute copies of free software (and charge for\
them if you wish), that you receive source code or can get it if you\
want it, that you can change the software or use pieces of it in new\
free programs, and that you know you can do these things.\n\
\n\
To protect your rights, we need to prevent others from denying you\
these rights or asking you to surrender the rights.  Therefore, you have\
certain responsibilities if you distribute copies of the software, or if\
you modify it: responsibilities to respect the freedom of others.\n\
\n\
For example, if you distribute copies of such a program, whether\
gratis or for a fee, you must pass on to the recipients the same\
freedoms that you received.  You must make sure that they, too, receive\
or can get the source code.  And you must show them these terms so they\
know their rights.\n\
\n\
Developers that use the GNU GPL protect your rights with two steps:\
(1) assert copyright on the software, and (2) offer you this License\
giving you legal permission to copy, distribute and/or modify it.\n\
\n\
For the developers\' and authors\' protection, the GPL clearly explains\
that there is no warranty for this free software.  For both users' and\
authors' sake, the GPL requires that modified versions be marked as\
changed, so that their problems will not be attributed erroneously to\
authors of previous versions.\n\
\n\
Some devices are designed to deny users access to install or run\
modified versions of the software inside them, although the manufacturer\
can do so.  This is fundamentally incompatible with the aim of\
protecting users' freedom to change the software.  The systematic\
pattern of such abuse occurs in the area of products for individuals to\
use, which is precisely where it is most unacceptable.  Therefore, we\
have designed this version of the GPL to prohibit the practice for those\
products.  If such problems arise substantially in other domains, we\
stand ready to extend this provision to those domains in future versions\
of the GPL, as needed to protect the freedom of users.\n\
\n\
Finally, every program is threatened constantly by software patents.\
States should not allow patents to restrict development and use of\
software on general-purpose computers, but in those that do, we wish to\
avoid the special danger that patents applied to a free program could\
make it effectively proprietary.  To prevent this, the GPL assures that\
patents cannot be used to render the program non-free.\n\
\n\
The precise terms and conditions for copying, distribution and\
modification follow.";

#endif
