//
// C++ Interface: BubbleGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2009-2010 Thibaut GRIDEL
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
#ifndef BUBBLE_H
#define BUBBLE_H

#include "boats.h"

#include <QObject>
#include <QGraphicsItem>

class PositionModel;

/**
    \class BubbleGraphicsItem

    \brief the QGraphicsItem for a bubble

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsItem for Item framework and QObject for slot and
    signal framework. It displays the text representation of a PositionModel on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, BoatModel

*/

class BubbleGraphicsItem : public QGraphicsTextItem {
        Q_OBJECT
    public:
        BubbleGraphicsItem(PositionModel *model, QGraphicsItem *parent = 0);
        virtual ~BubbleGraphicsItem();

        PositionModel* model() const { return m_model; }

        QRectF boundingRect() const;
        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget);
        void setVisible(bool visible);

    public slots:
        void updateText(QString value);
        void setText();
        void setPosition(QPointF position);
        void setTail();

    protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        virtual void focusOutEvent (QFocusEvent *event);

    private:
        /// \a m_model holds the PositionModel being represented
        PositionModel *m_model;

        /// \a m_fromPosition holds the position of the mousePress
        QPointF m_fromPosition;

        /// \a m_tail holds the leader line to the boat
        QGraphicsPolygonItem *m_tail;

        QTime m_clickTime;
};

#endif // BUBBLE_H
