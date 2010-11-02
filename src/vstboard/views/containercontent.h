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

#ifndef CONTAINERCONTENT_H
#define CONTAINERCONTENT_H

#include "precomp.h"

namespace View {

    class ContainerContent : public QGraphicsWidget
    {
    Q_OBJECT
    public:
        explicit ContainerContent(QAbstractItemModel *model, QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0);
        void SetModelIndex(QPersistentModelIndex index);
        QPointF GetDropPos();
    protected:
        void dragEnterEvent( QGraphicsSceneDragDropEvent *event);
        void dropEvent( QGraphicsSceneDragDropEvent *event);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

        QAbstractItemModel *model;
        QPersistentModelIndex objIndex;
        QPointF dropPos;
    signals:

    public slots:

    };
}
#endif // CONTAINERCONTENT_H
