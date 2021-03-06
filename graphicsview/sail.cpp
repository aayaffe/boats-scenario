//
// C++ Implementation: SailGraphicsItem
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
#include "sail.h"

#include "commontypes.h"
#include "trackmodel.h"
#include "boatmodel.h"

#include <QPainter>
#include <QGraphicsScene>

#include <iostream>

extern int debugLevel;

SailGraphicsItem::SailGraphicsItem(BoatModel *boat, QGraphicsItem *parent)
        : QGraphicsPathItem(parent),
        m_boat(boat),
        m_sailAngle(0),
        m_sailSize(0) {
    setZValue(0);
    setBrush(QBrush(Qt::white));
}


SailGraphicsItem::~SailGraphicsItem() {}

void SailGraphicsItem::setPosition(QPointF position) {
    if (pos() != position) {
        setPos(position);
        update();
    }
}

void SailGraphicsItem::setSailSize(qreal sailSize) {
    m_sailSize = sailSize;
        QPainterPath sailPathStalled;
        sailPathStalled.cubicTo(.1 * sailSize, .2 * sailSize, .1 * sailSize, .2 * sailSize, 0, .3 * sailSize);
        sailPathStalled.cubicTo(-.1 * sailSize, .4 * sailSize, -.1 * sailSize, .4 * sailSize, 0, .5 * sailSize);
        sailPathStalled.cubicTo(.1 * sailSize, .6 * sailSize, .1 * sailSize, .6 * sailSize, 0, .7 * sailSize);
        sailPathStalled.cubicTo(-.1 * sailSize, .8 * sailSize, -.1 * sailSize, .8 * sailSize, 0, sailSize);
        sailPathStalled.lineTo(0, 0);
        m_sailPathStalled = sailPathStalled;


        QPainterPath sailPathStarboard;
        sailPathStarboard.cubicTo(.1 * sailSize, .4 * sailSize, .1 * sailSize, .6 * sailSize, 0, sailSize);
        sailPathStarboard.lineTo(0, 0);
        m_sailPathStarboard = sailPathStarboard;

        QPainterPath sailPathPort;
        sailPathPort.cubicTo(-.1 * sailSize, .4 * sailSize, -.1 * sailSize, .6 * sailSize, 0, sailSize);
        sailPathPort.lineTo(0, 0);
        m_sailPathPort = sailPathPort;

        setSailAngle(m_sailAngle); // Need to call setSailAngle() to update path, but don't want this to be sail specific. But not looking to change sail angle per se, so no need to use anything other than m_sailAngle.
}

/// calculate a sail incidence angle, corrected with user trimming
void SailGraphicsItem::setSailAngle(qreal value) {
    m_sailAngle = value;
    qreal angle = fmod(m_boat->heading() - m_boat->wind() - m_sailAngle +360, 360);
    if(angle < 0) angle +=360;

    if ((angle < 10 || angle > 350 || (angle > 170 && angle < 190)) && path() != m_sailPathStalled) {
        setPath(m_sailPathStalled);
    } else if (angle >= 10 && angle <= 170 && path() != m_sailPathStarboard) {
        setPath(m_sailPathStarboard);
    } else if (angle >= 190 && angle <= 350 && path() != m_sailPathPort) {
        setPath(m_sailPathPort);
    }

    QTransform transform;
    transform.rotate(- m_sailAngle);
    setTransform(transform, false);
}
