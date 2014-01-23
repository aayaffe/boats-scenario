//
// C++ Interface: SpinnakerGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2010 Thibaut GRIDEL
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
#ifndef SPINNAKER_H
#define SPINNAKER_H

#include "boats.h"
#include "sail.h"

#include <QObject>
#include <QGraphicsPathItem>

class BoatModel;

/**
    \class SpinnakerGraphicsItem

    \brief the QGraphicsItem for a sail

    The class represents the Item according to the Graphics View Framework.

    It inherits SailGraphicsItem which provides necessary update framework.
    It displays the spin representation of a BoatModel on a SituationView.

    \sa SituationView, SituationScene, SituationModel, BoatModel

*/

class SpinnakerGraphicsItem : public SailGraphicsItem {
        Q_OBJECT
    public:
        SpinnakerGraphicsItem(BoatModel *boat, QGraphicsItem *parent = 0);
        virtual ~SpinnakerGraphicsItem();

        void setSailSize(qreal size);

    public slots:
        void setSailAngle(qreal value);
        void setHeading(qreal value);

    private:
        /// \a m_pole holds the spinnaker pole
        QGraphicsLineItem *m_pole;
};

#endif // SPINNAKER_H
