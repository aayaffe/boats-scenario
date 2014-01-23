//
// C++ Implementation: GennakerGraphicsItem
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
#include "gennaker.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"

#include <QPainter>
#include <QGraphicsScene>

#include <iostream>

extern int debugLevel;

GennakerGraphicsItem::GennakerGraphicsItem(BoatModel *boat, QGraphicsItem *parent)
        : SailGraphicsItem(boat, parent),
        m_pole(new QGraphicsLineItem(parent)) { //NB boat as parent to avoid need to offset sail rotatation for pole
}

GennakerGraphicsItem::~GennakerGraphicsItem() {}

void GennakerGraphicsItem::setSailSize(qreal sailSize) {
    m_sailSize = sailSize;
        QPainterPath sailPathStalled;
        sailPathStalled.cubicTo(.1 * sailSize, .2 * sailSize, .1 * sailSize, .2 * sailSize, 0, .3 * sailSize);
        sailPathStalled.cubicTo(-.1 * sailSize, .4 * sailSize, -.1 * sailSize, .4 * sailSize, 0, .5 * sailSize);
        sailPathStalled.cubicTo(.1 * sailSize, .6 * sailSize, .1 * sailSize, .6 * sailSize, 0, .7 * sailSize);
        sailPathStalled.cubicTo(-.1 * sailSize, .8 * sailSize, -.1 * sailSize, .8 * sailSize, 0, sailSize);
        sailPathStalled.lineTo(0, 0);
        m_sailPathStalled = sailPathStalled;

        QPainterPath sailPathStarboard;
        sailPathStarboard.cubicTo(.35 * sailSize, .3 * sailSize, .28 * sailSize, .6 * sailSize, 0, sailSize);
        sailPathStarboard.cubicTo(.2 * sailSize, .6 * sailSize, .25 * sailSize, .3 * sailSize, 0, 0);
        m_sailPathStarboard = sailPathStarboard;

        QPainterPath sailPathPort;
        sailPathPort.cubicTo(-.35 * sailSize, .3 * sailSize, -.28 * sailSize, .6 * sailSize, 0, sailSize);
        sailPathPort.cubicTo(-.2 * sailSize, .6 * sailSize, -.25 * sailSize, .3 * sailSize, 0, 0);
        m_sailPathPort = sailPathPort;

        setSailAngle(m_sailAngle);
}

void GennakerGraphicsItem::setPoleLength(qreal length) {
    m_pole->setLine(0,0,0,length);
    QPen polePen;
    polePen.setWidthF(0.5);
    polePen.setCapStyle(Qt::FlatCap);
    m_pole->setPen(polePen);
}

void GennakerGraphicsItem::setPosition(QPointF position) {
    if (pos() != position) {
        setPos(position);
        m_pole->setPos(position);
        update();
    }
}

void GennakerGraphicsItem::setVisible(bool visibility) {
    if (isVisible() != visibility) {
        SailGraphicsItem::setVisible(visibility);
        m_pole->setVisible(visibility);
        update();
    }
}

/// calculate a sail incidence angle, corrected with user trimming
void GennakerGraphicsItem::setSailAngle(qreal value) {
    m_sailAngle = value;
    qreal angle = fmod(m_boat->heading() - m_boat->wind() - m_sailAngle +360, 360);
    if(angle < 0) angle +=360;

    if ((angle < 55 || angle > 305 || (angle > 170 && angle < 190)) && path() != m_sailPathStalled) {
        setPath(m_sailPathStalled);
    } else if (angle >= 55 && angle <= 170 && path() != m_sailPathStarboard) {
        setPath(m_sailPathStarboard);
    } else if (angle >= 190 && angle <= 305 && path() != m_sailPathPort) {
        setPath(m_sailPathPort);
    }
    QTransform transform;
    transform.rotate(- m_sailAngle);
    setTransform(transform, false);
}
