//
// C++ Interface: LaylinesGraphicsItem
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
#ifndef LAYLINES_H
#define LAYLINES_H

#include <QGraphicsItem>
#include <QObject>

#include "boats.h"

class PositionModel;

/**
    \class LaylinesGraphicsItem

    \brief the QGraphicsItem for laylines

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsPathItem for Item framework and QObject for slot and
    signal framework. It displays the laylines of a PositionModel on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, PositionModel

*/

class LaylinesGraphicsItem : public QObject, public QGraphicsPathItem {
        Q_OBJECT
        Q_INTERFACES(QGraphicsItem)

    public:
        LaylinesGraphicsItem(PositionModel *model, QGraphicsItem *parent = 0);
        virtual ~LaylinesGraphicsItem();

        void updatePath();

    public slots:
        void setLength(int length);
        void setSeries(int value);
        void setLaylineAngle(int value);
        void setWind();
        void setVisible(bool visible);

    protected:
        /// \a m_model holds the PositionModel being represented
        PositionModel *m_model;

        /// \a m_length holds the number of boat lengths for zone
        int m_length;

        /// \a m_boatLength holds the size in scene coordinates for main series
        int m_boatLength;

        /// \a m_laylineAngle holds the layline angle
        int m_laylineAngle;

        qreal m_wind;

        bool m_visible;
};

#endif // LAYLINES_H
