//
// C++ Interface: BoatGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2011 Thibaut GRIDEL
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

#include "boats.h"
#include "sail.h"
#include "spinnaker.h"
#include "gennaker.h"
#include "flag.h"
#include "bubble.h"
#include "laylines.h"

#include <QObject>
#include <QColor>
#include <QGraphicsItem>

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
    Q_INTERFACES(QGraphicsItem)

    public:
        BoatGraphicsItem(BoatModel *boat, QGraphicsItem *parent = 0);
        virtual ~BoatGraphicsItem();

        BoatModel* boat() const { return m_boat; }
        BubbleGraphicsItem* bubble() const { return m_bubble; }

        QRectF boundingRect() const;
        QPainterPath shape() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget);
        virtual int type() const;

    public slots:
        void setHeading(qreal value);
        void setPosition(QPointF position);
        void setOrder(int value);
        void setSpin(bool value);
        void setOverlap(Boats::Overlaps value);
        void setDisplayFlag(Boats::Flag value);
        void setHidden(bool value);
        void setColor(QColor value);
        void setDim(int value);
        void setVisible(bool value);
        void setSeries(Boats::Series value);
        void deleteItem(BoatModel *boat);
        void setSelected(bool selected);

    protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    private:
        void setOverlapLine();

        /// \a m_boat holds the BoatModel being represented
        BoatModel *m_boat;

        /// \a m_hullPath holds the path for the hull
        QPainterPath m_hullPath;

        /// \a m_boundingRect holds the bounding rectangle for the boat
        QRectF m_boundingRect;

        /// \a m_angle holds the heading of the boat
        qreal m_angle;

        /// \a m_sail holds the sail that will be drawn
        SailGraphicsItem *m_sail;

        /// \a m_jib holds the jib
        SailGraphicsItem *m_jib;

        /// \a m_spin holds the spinnaker
        SpinnakerGraphicsItem *m_spin;

        /// \a m_genn holds the gennaker
        GennakerGraphicsItem *m_genn;

        /// \a m_hasSpin is true if boat has a spinnaker
        bool m_hasSpin;

        /// \a m_hasGenn is true if boat has a gennaker
        bool m_hasGenn;

        /// \a m_overlap holds whether an overlap line should be displayed
        Boats::Overlaps m_overlap;
        QGraphicsLineItem *m_overlapLine;

        /// \a m_border holds the half-width of the boat at the transom
        qreal m_border;

        /// \a m_color holds the color of the TrackModel
        QColor m_color;

        /// \a m_flag holds the flag to display
        Boats::Flag m_flag;
        FlagGraphicsItem *m_flagRect;

        /// \a m_hidden holds whether the hidden symbol should be displayed
        bool m_hidden;

        /// \a m_bubble holds the bubble to display
        BubbleGraphicsItem *m_bubble;

        /// \a m_series holds the series of the TrackModel
        Boats::Series m_series;

        /// \a m_order holds the stacking order in the TrackModel
        int m_order;

        /// \a m_numberPath holds the number path that will be drawn on the hull
        QGraphicsPathItem *m_numberPath;

        /// \a m_numberSize holds the font size of the number
        int m_numberSize;

        /// \a m_laylines holds the laylines for the boat
        LaylinesGraphicsItem *m_laylines;

        /// \a m_multiSelect is true if Ctrl-modified was in effect when mousePressEvent happened
        bool m_multiSelect;

        /// \a m_trackSelect is true if mousePressEvent is Shift-modified
        bool m_trackSelect;
};

#endif
