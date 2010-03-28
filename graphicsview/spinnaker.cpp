//
// C++ Implementation: SpinnakerGraphicsItem
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
#include <iostream>

#include <QPainter>
#include <QGraphicsScene>

#include "spinnaker.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"

extern int debugLevel;

SpinnakerGraphicsItem::SpinnakerGraphicsItem(BoatModel *boat, QGraphicsItem *parent)
        : SailGraphicsItem(boat, parent),
        m_pole(new QGraphicsLineItem(this)) {
}


SpinnakerGraphicsItem::~SpinnakerGraphicsItem() {}

void SpinnakerGraphicsItem::setSailSize(qreal sailSize) {
    m_sailSize = sailSize;
        QPainterPath sailPathStalled;
        sailPathStalled.cubicTo(.1 * sailSize, .2 * sailSize, .1 * sailSize, .2 * sailSize, 0, .3 * sailSize);
        sailPathStalled.cubicTo(-.1 * sailSize, .4 * sailSize, -.1 * sailSize, .4 * sailSize, 0, .5 * sailSize);
        sailPathStalled.cubicTo(.1 * sailSize, .6 * sailSize, .1 * sailSize, .6 * sailSize, 0, .7 * sailSize);
        sailPathStalled.cubicTo(-.1 * sailSize, .8 * sailSize, -.1 * sailSize, .8 * sailSize, 0, sailSize);
        sailPathStalled.lineTo(0, 0);
        m_sailPathStalled = sailPathStalled;


        QPainterPath sailPathStarboard;
        sailPathStarboard.cubicTo(.4 * sailSize, .2 * sailSize, .6 * sailSize, .2 * sailSize, sailSize, 0);
        sailPathStarboard.arcTo(-sailSize, -sailSize, 2 * sailSize, 2 * sailSize, 0, -120);
        sailPathStarboard.cubicTo(-.12 * sailSize, .62 * sailSize, -.03 * sailSize, .44 * sailSize, 0, 0);
        m_sailPathStarboard = sailPathStarboard;

        QPainterPath sailPathPort;
        sailPathPort.cubicTo(-.4 * sailSize, .2 * sailSize, -.6 * sailSize, .2 * sailSize, -sailSize, 0);
        sailPathPort.arcTo(- sailSize, -sailSize, 2 * sailSize, 2 * sailSize, 180, 120);
        sailPathPort.cubicTo(.12 * sailSize, .62 * sailSize, .03 * sailSize, .44 * sailSize, 0, 0);
        m_sailPathPort = sailPathPort;

        QLineF line;
        line.setP2(QPointF(0, -sailSize));
        m_pole->setLine(line);

        setSailAngle(m_boat->spinAngle() + m_boat->spinTrim());
}

/// calculate a sail incidence angle, corrected with user trimming
void SpinnakerGraphicsItem::setSailAngle(qreal value) {
    m_sailAngle = value;
    qreal angle = fmod(m_boat->heading() - m_sailAngle +360, 360);

    if ((angle < 10 || angle > 350 || (angle > 170 && angle < 190)) && path() != m_sailPathStalled) {
        setPath(m_sailPathStalled);
    } else if (angle >= 10 && angle <= 170 && path() != m_sailPathStarboard) {
        setPath(m_sailPathStarboard);
    } else if (angle >= 190 && angle <= 350 && path() != m_sailPathPort) {
        setPath(m_sailPathPort);
    }
}

void SpinnakerGraphicsItem::setHeading(qreal value) {
    qreal angle = value;
    QTransform rotation;
    rotation.rotate(-angle);
    setTransform(rotation, false);

    QTransform poleRotation;
    if (angle > 90 && angle < 180) {
        poleRotation.rotate(90);
    } else if (angle >= 180 && angle < 270) {
        poleRotation.rotate(-90);
    } else {
        poleRotation.rotate(angle);
    }
    m_pole->setTransform(poleRotation, false);
}
