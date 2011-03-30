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

#ifndef PINVIEW_H
#define PINVIEW_H

#include "precomp.h"
#include "globals.h"
#include "connectables/connectioninfo.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4100 )
#endif

namespace Connectables {
    class Pin;
}

namespace View {
    class CableView;
    class ConnectableObjectView;
    class PinView : public QGraphicsWidget
    {
    Q_OBJECT

    public:

        explicit PinView(float angle, QAbstractItemModel *model,QGraphicsItem * parent, const ConnectionInfo &pinInfo);
        const ConnectionInfo GetConnectionInfo() const {return connectInfo;}
        void AddCable(CableView *cable);
        void RemoveCable(CableView *cable);
        void UpdateCablesPosition();

        /*!
          Called when the model changed
          \param index the model index of the pin
          \todo does the parameter can be removed since the index is stored anyway ?
          */
        virtual void UpdateModelIndex(const QModelIndex &index) {}

        /*!
          Set the model index and update the view
          \param index the new model index
          */
        virtual void SetPinModelIndex(QPersistentModelIndex index) {pinIndex = index; UpdateModelIndex(index);}

        virtual const QPointF pinPos() const;

    protected:

        QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
        void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        void mouseMoveEvent ( QGraphicsSceneMouseEvent  * event );
        void mouseReleaseEvent ( QGraphicsSceneMouseEvent  * event );
        void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
        void dragLeaveEvent( QGraphicsSceneDragDropEvent  * event );
        void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
        void dropEvent ( QGraphicsSceneDragDropEvent  * event );

        void CreateMimeData(QByteArray &bytes);
        void ReadMimeData(QByteArray &bytes, ConnectionInfo &data);

        /// background
        /// \todo is it used ?
        QAbstractGraphicsShapeItem *rectBgnd;

        /// highlight brush on mouseover
        static QBrush highlightBrush;

        /// list of connected cables
        QList<CableView *> connectedCables;

        /// description of the pin
        ConnectionInfo connectInfo;

        /// pointer to the model
        QAbstractItemModel *model;

        /// model index of the pin
        QPersistentModelIndex pinIndex;

        /// pin angle in rad
        float pinAngle;

        /// temporary cable for drag&drop
        static QGraphicsLineItem *currentLine;

    signals:
        /*!
          emitted when a pin is drag&droped over another one
          \param outputPin
          \param inputPin
          */
        void ConnectPins(ConnectionInfo outputPin, ConnectionInfo inputPin);

        /*!
          emitted on double click to remove all the connected cables
          */
        void RemoveCablesFromPin(ConnectionInfo pin);

    public slots:
        /// update the vu-meter, called by a timer
        virtual void updateVu(){}

    friend class Cable;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif // PINVIEW_H