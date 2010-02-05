//
// C++ Implementation: SailGraphicsItem
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
#include <iostream>

#include <QPainter>
#include <QGraphicsScene>

#include "sail.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"

extern int debugLevel;

SailGraphicsItem::SailGraphicsItem(BoatModel *boat, QGraphicsItem *parent)
        : QGraphicsPathItem(parent),
        m_boat(boat) {
    setZValue(0);

    setHeading(boat->heading());
    setTrim(boat->trim());
    setSailAngle();

    connect(boat, SIGNAL(headingChanged(qreal)),
            this, SLOT(setHeading(qreal)));
    connect(boat, SIGNAL(trimChanged(qreal)),
            this, SLOT(setTrim(qreal)));
}


SailGraphicsItem::~SailGraphicsItem() {}

void SailGraphicsItem::setPosition(QPointF position) {
    if (pos() != position) {
        setPos(position);
        update();
    }
}
void SailGraphicsItem::setSailSize(qreal sailSize) {
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

        setSailAngle();
}

void SailGraphicsItem::setHeading(qreal value) {
    if (m_angle != value) {
        m_angle = value;
        setSailAngle();
        update();
    }
}

void SailGraphicsItem::setTrim(qreal value) {
    if (m_trim != value) {
        m_trim = value;
        setSailAngle();
        update();
    }
}

/// calculate a sail incidence angle, corrected with user trimming
void SailGraphicsItem::setSailAngle() {
    m_sailAngle = m_boat->getSailAngle(m_boat->track()->situation()->laylineAngle(), m_angle, m_boat->track()->series(), m_trim);
    qreal angle = fmod(m_angle - m_sailAngle +360, 360);

    if ((angle < 10 || angle > 350 || (angle > 170 && angle < 190)) && path() != m_sailPathStalled) {
        setPath(m_sailPathStalled);
    } else if (angle >= 10 && angle <= 170 && path() != m_sailPathStarboard) {
        setPath(m_sailPathStarboard);
    } else if (angle >= 190 && angle <= 350 && path() != m_sailPathPort) {
        setPath(m_sailPathPort);
    }

    QTransform transform;
//    transform.translate(m_mast.x(), m_mast.y());
    transform.rotate(- m_sailAngle);
    setTransform(transform, false);
}
