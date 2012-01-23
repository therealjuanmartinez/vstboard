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

#ifndef MAINHOSTHOST_H
#define MAINHOSTHOST_H

#include "mainhost.h"
#include "audiodevices.h"
#include "mididevices.h"

class AudioDevices;
class MidiDevices;
class MainHostHost : public MainHost
{
Q_OBJECT
public:
    MainHostHost(Settings *settings, QObject *parent = 0);
    ~MainHostHost();
    void SendMsg(const MsgObject &msg);

    AudioDevices *audioDevices;
    MidiDevices *midiDevices;

signals:
    void SendMsgSignal(const MsgObject &msg);

public slots:
    void Render();
    void ReceiveMsgSignal(const MsgObject &msg);
};

#endif // MAINHOSTHOST_H

