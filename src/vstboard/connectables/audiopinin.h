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

#ifndef CONNECTABLEAUDIOPININ_H
#define CONNECTABLEAUDIOPININ_H

#include "pin.h"

class AudioBuffer;

namespace Connectables {

    class AudioPinIn : public Pin
    {
    public:
        AudioPinIn(Object *parent, int number, bool externalAllocation = false, bool bridge=false);
        ~AudioPinIn();

        void SetBuffer(AudioBuffer *buffer);
//        void ProcessAudio();
        void ReceiveMsg(const int msgType=0,void *data=0);
        AudioBuffer *buffer;
        float GetValue();
        void NewRenderLoop();
    protected:

    };
}
#endif // CONNECTABLEAUDIOPININ_H
