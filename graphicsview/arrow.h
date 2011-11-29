//
// C++ Interface: ArrowGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2011 Thibaut GRIDEL
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
#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsItem>
#include <QObject>
#include <QColor>

class WindModel;

/**
    \class ArrowGraphicsItem

    \brief the QGraphicsItem for a wind Arrow

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsItem for Item framework and QObject for slot and
    signal framework. It displays the representation of an arrow on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, WindModel

*/

class ArrowGraphicsItem : public QObject, public QGraphicsItem {
        Q_OBJECT
        Q_INTERFACES(QGraphicsItem)
    public:
        ArrowGraphicsItem(WindModel *wind = 0, QGraphicsItem *parent = 0);
        ~ArrowGraphicsItem();

        virtual QRectF boundingRect() const;
        virtual QPainterPath shape() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget);
        virtual int type() const;

    public slots:
        void resetArrow();
        void setHeading(qreal value);
        void setPosition(QPointF position);
        void deleteItem(bool visible);

    private:
        WindModel *m_wind;

        /// \a m_angle holds the angle of the Arrow
        qreal m_angle;

        QPainterPath m_path;
};

#endif
