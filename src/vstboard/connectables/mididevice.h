/******************************************************************************
#    Copyright 2010 Rapha�l Fran�ois
#    Contact : ctrlbrk76@gmail.com
#
#    This file is part of VstBoard.
#
#    VstBoard is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    VstBoard is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with VstBoard.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef MIDIDEVICE_H
#define MIDIDEVICE_H

#include "../precomp.h"
#include "object.h"

#define QUEUE_SIZE 1024

namespace Connectables {

    class MidiDevice : public Object
    {
    Q_OBJECT

    public:
        MidiDevice(int index, const ObjectInfo &info);
        ~MidiDevice();
        bool Open();
        bool Close();
        void Render();
        void MidiMsgFromInput(long msg);

        PmStream *stream;
        PmQueue *queue;

        const PmDeviceInfo *devInfo;

    protected:
        bool FindDeviceFromName();

    signals:
        void SendMsg(int msgType,void *data=0);
    };

}
#endif // MIDIDEVICE_H
