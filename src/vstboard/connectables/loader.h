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

#ifndef LOADER_H
#define LOADER_H

#include "precomp.h"

namespace Connectables {
    class Object;
    class Loader : public QThread
    {
        Q_OBJECT
    public:
        explicit Loader(QSharedPointer<Object> objToLoad, QObject *parent = 0);
        ~Loader();

    protected:
        void run();
        QSharedPointer<Object> objToLoad;
    signals:

    public slots:

    };
}
#endif // LOADER_H
