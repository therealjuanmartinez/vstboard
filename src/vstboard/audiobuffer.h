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

#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#define BLANK_BUFFER_SIZE 1024

#include "precomp.h"

class AudioBuffer
{
public:
        AudioBuffer(bool externalAllocation = false);
        ~AudioBuffer(void);
        bool SetSize(long size);
        void Clear();
//        void MixWith(AudioBuffer * buff, float mult=1.0f);
        void AddToStack(AudioBuffer * buff);
//        void SetGain(float gain);
        void SetPointer(float * buff, bool tmpBufferToBeFilled=false);
        float *GetPointer(bool willBeFilled=false);
        float *ConsumeStack();
        inline long GetSize() {return nSize;}
//        void CopyFrom(AudioBuffer * buff);
//        void InvalidateVu();
        float GetVu();
        inline void ResetStackCounter() {stackSize=0;}

protected:
        int stackSize;
        float * pBuffer;
        long nSize;
        long nAllocatedSize;
        float _maxVal;
        static float blankBuffer[BLANK_BUFFER_SIZE];
        bool bExternalAllocation;
};


#endif // AUDIOBUFFER_H
