//
// C++ Interface: PointGraphicsItem
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
#ifndef POINT_H
#define POINT_H

#include <QGraphicsItem>
#include <QObject>

#include "commontypes.h"
#include "bubble.h"

class PointModel;

/**
    \class PointGraphicsItem

    \brief the QGraphicsItem for a point

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsItem for Item framework and QObject for slot and
    signal framework. It displays the representation of a PositionModel on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, PositionModel

*/

class PointGraphicsItem : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    public:
        PointGraphicsItem(PointModel *point = 0, QGraphicsItem *parent = 0);
        ~PointGraphicsItem();

        PointModel* point() const { return m_point; }
        BubbleGraphicsItem* bubble() const { return m_bubble; }

        QRectF boundingRect() const;
        QPainterPath shape() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget);
        int type() const;

    public slots:
        void setPosition(QPointF position);
        void deleteItem(PointModel *point);

    private:
        /// \a m_point holds the PointModel being represented
        PointModel *m_point;

        /// \a m_bubble holds the bubble to display
        BubbleGraphicsItem *m_bubble;

        /// \a m_selected holds selection information
        bool m_selected;

        QPolygon m_points;
};

#endif
