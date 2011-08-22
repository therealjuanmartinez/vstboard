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

#ifndef CONTAINERCONTENT_H
#define CONTAINERCONTENT_H

//#include "precomp.h"
#include "mainwindow.h"
#include "objectdropzone.h"

class SceneModel;
namespace View {
    class MainContainerView;
    class ContainerContent : public ObjectDropZone, public MetaData
    {
    Q_OBJECT
    public:
        explicit ContainerContent(const MetaData &info, MainContainerView * parent, SceneModel *model );
//        void SetModelIndex(const MetaData &info);
//        QPointF GetDropPos();
//        void SetDropPos(const QPointF &pt);
        void SetConfig(ViewConfig *config);

    protected:
        void dragEnterEvent( QGraphicsSceneDragDropEvent *event);
        void dragMoveEvent( QGraphicsSceneDragDropEvent *event);
        void dragLeaveEvent( QGraphicsSceneDragDropEvent *event);
        void dropEvent( QGraphicsSceneDragDropEvent *event);

//        QPointF dropPos;

        QPersistentModelIndex attachLeft;
        QPersistentModelIndex attachRight;
        QGraphicsRectItem *rectAttachLeft;
        QGraphicsRectItem *rectAttachRight;

        SceneModel *model;

        ViewConfig *config;
    signals:

    public slots:
        void UpdateColor(ColorGroups::Enum groupId, Colors::Enum colorId, const QColor &color);
        void HighlightStart();
        void HighlightStop();
    };
}
#endif // CONTAINERCONTENT_H