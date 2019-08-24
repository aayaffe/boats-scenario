//
// C++ Interface: MarkGraphicsItem
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
#ifndef MARK_H
#define MARK_H

#include "commontypes.h"
#include "bubble.h"
#include "laylines.h"

#include <QObject>
#include <QColor>
#include <QGraphicsItem>

class MarkModel;

/**
    \class MarkGraphicsItem

    \brief the QGraphicsItem for a mark

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsItem for Item framework and QObject for slot and
    signal framework. It displays the representation of a MarkModel on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, MarkModel

*/

class MarkGraphicsItem : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    public:
        MarkGraphicsItem(MarkModel *mark = 0, QGraphicsItem *parent = 0);
        ~MarkGraphicsItem();

        MarkModel* mark() const { return m_mark; }
        BubbleGraphicsItem* bubble() const { return m_bubble; }

        QRectF boundingRect() const;
        QPainterPath shape() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget);
        int type() const;

    public slots:
        void setPosition(QPointF position);
        void setOrder(int value);
        void setColor(QColor value);
        void setZone(bool zone);
        void setLength(int length);
        void setSeries(int value);
        void deleteItem(MarkModel *mark);
        void setHeading(qreal heading);
        void setArrowVisible(bool visible);
        void setLeaveToPort(bool leaveToPort);
        void setLabelVisible(bool visible);
        void setLabelText(QString text);

    protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    private:
        /// \a m_mark holds the MarkModel being represented
        MarkModel *m_mark;

        /// \a m_color holds the color of the mark
        QColor m_color;

        /// \a m_zone holds wether zone is displayed
        bool m_zone;

        /// \a m_length holds the number of boat lengths for zone
        int m_length;

        /// \a m_boatLength holds the size in scene coordinates for main series
        int m_boatLength;

        /// \a m_bubble holds the bubble to display
        BubbleGraphicsItem *m_bubble;

        /// \a m_order holds the stacking order in the mark list
        int m_order;

        /// \a m_laylines holds the laylines for the mark
        LaylinesGraphicsItem *m_laylines;

        /// \a m_heading holds the orientation of the arrow
        qreal m_heading;

        /// \a m_arrowVisible holds wether the mark arrow is visible
        bool m_arrowVisible;

        /// \a m_leaveToPort holds the orientation of the arrow
        bool m_leaveToPort;

        /// \a m_multiSelect is true if Ctrl-modified was in effect when mousePressEvent happened
        /// Need to save this state until receive mouseReleaseEvent to determine what to do
        bool m_multiSelect;

        bool m_labelVisible;
        QString m_labelText;
};

#endif
