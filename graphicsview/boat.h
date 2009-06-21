//
// C++ Interface: BoatGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2009 Thibaut GRIDEL
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
#ifndef BOAT_H
#define BOAT_H

#include <QGraphicsItem>
#include <QObject>
#include <QColor>

#include "boats.h"
#include "flag.h"

class BoatModel;

/**
    \class BoatGraphicsItem

    \brief the QGraphicsItem for a boat

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsItem for Item framework and QObject for slot and
    signal framework. It displays the representation of a BoatModel on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, BoatModel

*/

class BoatGraphicsItem : public QObject, public QGraphicsItem {
        Q_OBJECT
    public:
        BoatGraphicsItem(BoatModel *boat, QGraphicsItem *parent = 0);
        virtual ~BoatGraphicsItem();

        BoatModel* boat() const { return m_boat; }

        QRectF boundingRect() const;
        QPainterPath shape() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget);
        virtual int type() const;

    public slots:
        void setHeading(qreal value);
        void setPosition(QPointF position);
        void setOrder(int value);
        void setTrim(qreal value);
        void setOverlap(Boats::Overlaps value);
        void setDisplayFlag(Boats::Flag value);
        void setColor(QColor value);
        void setSeries(Boats::Series value);
        void deleteItem(BoatModel *boat);

    protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    private:
        void setSailAngle();
        void setOverlapLine();

        /// \a m_boat holds the BoatModel being represented
        BoatModel *m_boat;

        /// \a m_hullPath holds the path for the hull
        QPainterPath m_hullPath;

        /// \a m_angle holds the heading of the boat
        qreal m_angle;

        /// \a m_sailAngle holds the ideal sail trimming angle
        qreal m_sailAngle;

        /// \a m_mast holds the position of the mast
        QPointF m_mast;

        /// \a m_sail holds the sail that will be drawn
        QGraphicsPathItem *m_sail;

        /// \a m_sailPathPort holds the sail path when on port tack
        QPainterPath m_sailPathPort;

        /// \a m_sailPathStarboard holds the sail path when on starboard tack
        QPainterPath m_sailPathStarboard;

        /// \a m_sailPathStalled holds the sail path when head to wind
        QPainterPath m_sailPathStalled;

        /// \a m_trim holds the manual trimming angle override
        qreal m_trim;

        /// \a m_overlap holds whether an overlap line should be displayed
        Boats::Overlaps m_overlap;
        QGraphicsLineItem *m_overlapLine;

        /// \a m_color holds the color of the TrackModel
        QColor m_color;

        /// \a m_flag holds the flag to display
        Boats::Flag m_flag;
        FlagGraphicsItem *m_flagRect;

        /// \a m_series holds the series of the TrackModel
        Boats::Series m_series;

        /// \a m_selected holds selection information
        bool m_selected;

        /// \a m_order holds the stacking order in the TrackModel
        int m_order;

        /// \a m_numberPath holds the number path that will be drawn on the hull
        QGraphicsPathItem *m_numberPath;

        /// \a m_numberSize holds the font size of the number
        int m_numberSize;
};

#endif
