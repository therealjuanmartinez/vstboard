/**************************************************************************
#    Copyright 2010-2011 Rapha�l Fran�ois
#    Contact : ctrlbrk76@gmail.com
#
#    This file is part of VstBoard.
#
#    VstBoard is free software: you can redistribute it and/or modify
#    it under the terms of the under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    VstBoard is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    under the terms of the GNU Lesser General Public License for more details.
#
#    You should have received a copy of the under the terms of the GNU Lesser General Public License
#    along with VstBoard.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#include "heap.h"
#ifndef QT_NO_DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

#include "listaudiodevicesview.h"
#include "globals.h"

ListAudioDevicesView::ListAudioDevicesView(QWidget *parent) :
    QTreeView(parent)
{
    audioDevConfig = new QAction(QIcon(":/img16x16/configure.png"), tr("Config device"), this);
    connect(audioDevConfig, SIGNAL(triggered()),
            this, SLOT(ConfigCurrentDevice()));

    updateList = new QAction(QIcon(":/img16x16/viewmag+.png"), tr("Refresh list"), this);
    connect(updateList, SIGNAL(triggered()),
            this, SIGNAL(UpdateList()));

    connect(this, SIGNAL(customContextMenuRequested( const QPoint& )),
            this, SLOT(AudioDevContextMenu(const QPoint &)));
}

void ListAudioDevicesView::AudioDevContextMenu(const QPoint &pt)
{
    currentDevice = indexAt(pt).sibling(0,0);

    QList<QAction *> lstActions;

    if(currentDevice.data(UserRoles::objInfo).isValid())
        lstActions << audioDevConfig;

    lstActions << updateList;
    QMenu::exec(lstActions, mapToGlobal(pt));
}

void ListAudioDevicesView::ConfigCurrentDevice()
{
    emit Config(currentDevice);
}
