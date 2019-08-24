//
// C++ Interface: GennakerGraphicsItem
//
// Description:
//
//
// Author: Graham Louth <boatscenario@louths.org.uk>
//
// Copyright (c) 2013 Graham Louth
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

#ifndef GENNAKER_H
#define GENNAKER_H

#include "boats.h"
#include "sail.h"

#include <QObject>
#include <QGraphicsPathItem>

class BoatModel;

/**
    \class GennakerGraphicsItem

    \brief the QGraphicsItem for a gennaker

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsPathItem for Item framework and QObject for slot and
    signal framework. It displays the sail representation of a BoatModel on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, BoatModel

*/

class GennakerGraphicsItem : public SailGraphicsItem {
        Q_OBJECT
    public:
        GennakerGraphicsItem(BoatModel *boat, QGraphicsItem *parent = 0);
        virtual ~GennakerGraphicsItem();

        void setSailSize(qreal size);
        void setPoleLength(qreal length);
        void setPosition(QPointF position);
        void setVisible(bool visibility);

    public slots:
        void setSailAngle(qreal value);

    private:
        QGraphicsLineItem *m_pole;

};

#endif // GENNAKER_H
