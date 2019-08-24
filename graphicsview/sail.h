//
// C++ Interface: SailGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2010 Thibaut GRIDEL
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
#ifndef SAIL_H
#define SAIL_H

#include "boats.h"

#include <QObject>
#include <QGraphicsPathItem>

class BoatModel;

/**
    \class SailGraphicsItem

    \brief the QGraphicsItem for a sail

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsPathItem for Item framework and QObject for slot and
    signal framework. It displays the sail representation of a BoatModel on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, BoatModel

*/

class SailGraphicsItem : public QObject, public QGraphicsPathItem {
        Q_OBJECT
    public:
        SailGraphicsItem(BoatModel *boat, QGraphicsItem *parent = 0);
        virtual ~SailGraphicsItem();

        BoatModel* boat() const { return m_boat; }

        void setPosition(QPointF position);
        void setSailSize(qreal size);

    public slots:
        void setSailAngle(qreal value);

    protected:
        /// \a m_boat holds the BoatModel being represented
        BoatModel *m_boat;

        /// \a m_sailAngle holds the ideal sail trimming angle
        qreal m_sailAngle;

        /// \a m_sailSize holds the size of the foot
        qreal m_sailSize;

        /// \a m_sailPathPort holds the sail path when on port tack
        QPainterPath m_sailPathPort;

        /// \a m_sailPathStarboard holds the sail path when on starboard tack
        QPainterPath m_sailPathStarboard;

        /// \a m_sailPathStalled holds the sail path when head to wind
        QPainterPath m_sailPathStalled;

};

#endif // SAIL_H
