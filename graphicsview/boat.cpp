//
// C++ Implementation: BoatGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2009 Thibaut GRIDEL
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
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include "boat.h"

#include "commontypes.h"
#include "situationscene.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"

extern int debugLevel;

BoatGraphicsItem::BoatGraphicsItem(BoatModel *boat, QGraphicsItem *parent)
        : QGraphicsItem(parent),
        m_boat(boat),
        m_angle(0),
        m_sail(new QGraphicsPathItem(this)),
        m_trim(boat->trim()),
        m_overlap(Boats::none),
        m_overlapLine(new QGraphicsLineItem(this)),
        m_color(boat->track()->color()),
        m_series(Boats::unknown),
        m_selected(false),
        m_order(0),
        m_numberPath(new QGraphicsPathItem(this)) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    setBoundingRegionGranularity(1);
    m_numberPath->setZValue(1);
    m_sail->setZValue(3);

    m_numberPath->setBrush(QBrush(Qt::black));
    m_sail->setBrush(QBrush(Qt::white));

    QPen dashPen(Qt::CustomDashLine);
    QVector<qreal> dashes;
    dashes << 5 << 5;
    dashPen.setDashPattern(dashes);
    m_overlapLine->setPen(dashPen);

    setSeries(boat->track()->series());
    setHeading(boat->heading());
    setSailAngle();
    setPos(boat->position());
    setOrder(boat->order());
    setOverlap(boat->overlap());

    connect(boat, SIGNAL(headingChanged(qreal)),
            this, SLOT(setHeading(qreal)));
    connect(boat, SIGNAL(positionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
    connect(boat, SIGNAL(orderChanged(int)),
            this, SLOT(setOrder(int)));
    connect(boat, SIGNAL(trimChanged(qreal)),
            this, SLOT(setTrim(qreal)));
    connect(boat, SIGNAL(overlapChanged(Boats::Overlaps)),
            this, SLOT(setOverlap(Boats::Overlaps)));
    connect(boat->track(), SIGNAL(colorChanged(QColor)),
            this, SLOT(setColor(QColor)));
    connect(boat->track(), SIGNAL(seriesChanged(Boats::Series)),
            this, SLOT(setSeries(Boats::Series)));
    connect(boat->track()->situation(), SIGNAL(boatRemoved(BoatModel*)),
            this, SLOT(deleteItem(BoatModel*)));
}


BoatGraphicsItem::~BoatGraphicsItem() {}

void BoatGraphicsItem::setHeading(qreal value) {
    if (m_angle != value) {
        prepareGeometryChange();
        m_angle = value;
        setSailAngle();
        update();
        QTransform rotation;
        rotation.rotate(m_angle),
        setTransform(rotation, false);
    }
}

/// calculate a sail incidence angle, corrected with user trimming
void BoatGraphicsItem::setSailAngle() {
    m_sailAngle = m_boat->getSailAngle(m_boat->track()->situation()->laylineAngle(), m_angle, m_series, m_trim);
    qreal angle = fmod(m_angle - m_sailAngle +360, 360);

    if ((angle < 10 || angle > 350 || (angle > 170 && angle < 190)) && m_sail->path() != m_sailPathStalled) {
        m_sail->setPath(m_sailPathStalled);
    } else if (angle >= 10 && angle <= 170 && m_sail->path() != m_sailPathStarboard) {
        m_sail->setPath(m_sailPathStarboard);
    } else if (angle >= 190 && angle <= 350 && m_sail->path() != m_sailPathPort) {
        m_sail->setPath(m_sailPathPort);
    }

    QTransform transform;
    transform.translate(m_mast.x(), m_mast.y());
    transform.rotate(- m_sailAngle);
    m_sail->setTransform(transform, false);
}

void BoatGraphicsItem::setPosition(QPointF position) {
    if (pos() != position) {
        setPos(position);
        update();
    }
}

void BoatGraphicsItem::setOrder(int value) {
    m_order = value;
    if (m_order && m_numberSize) {
        QString number = QString::number(m_order);
        QFont numberFont;
        numberFont.setPointSize(m_numberSize);
        QFontMetrics fm(numberFont);
        QPainterPath fpath;
        fpath.addText(-fm.width(number)/2.0, 0, numberFont, number);
        m_numberPath->setPath(fpath);
    } else {
        m_numberPath->setPath(QPainterPath());
    }
    setZValue(m_order);
    update();
}

void BoatGraphicsItem::setTrim(qreal value) {
    if (m_trim != value) {
        m_trim = value;
        setSailAngle();
        update();
    }
}

void BoatGraphicsItem::setOverlap(Boats::Overlaps value) {
    if (m_overlap != value) {
        m_overlap = value;
        setOverlapLine();
    }
}

void BoatGraphicsItem::setOverlapLine() {
    qreal size = m_boat->track()->situation()->sizeForSeries(m_series);
    qreal border;
    switch(m_series) {
        case Boats::keelboat:
            border = 10;
            break;
        case Boats::laser:
            border = 5;
            break;
        case Boats::optimist:
            border = 4.6;
            break;
        case Boats::tornado:
            border = 14.7;
            break;
        case Boats::startboat:
            border = 17;
            break;
        default:
            border = 0;
            break;
    }
    QLineF line;
    line.setP1(QPointF(border, size/2));
    line.setP2(QPointF(-border, size/2));
    if (m_overlap == Boats::none) {
        m_overlapLine->setVisible(false);
    } else {
        if (m_overlap & Boats::starboard) {
            line.setP2(QPointF(border + size, size/2));
        }
        if (m_overlap & Boats::port) {
            line.setP1(QPointF(-border - size, size/2));
        }
        m_overlapLine->setVisible(true);
        m_overlapLine->setLine(line);
    }
}

void BoatGraphicsItem::setColor(QColor value) {
    if (m_color != value) {
        m_color = value;
        update();
    }
}

void BoatGraphicsItem::setSeries(Boats::Series value) {
    if (m_series != value) {
        prepareGeometryChange();
        m_series = value;

        int posY = 0;
        qreal sailSize = 0;

        switch (m_series) {
        case Boats::keelboat:
            m_numberSize = 12;
            posY = 25;
            m_mast = QPointF(0, -8.7);
            sailSize = 41.5;
            break;
        case Boats::laser:
            m_numberSize = 7;
            posY = 10;
            m_mast = QPointF(0, -8.7);
            sailSize = 28.5;
            break;
        case Boats::optimist:
            m_numberSize = 6;
            posY = 3;
            m_mast = QPointF(0,-6.9);
            sailSize = 16.5;
            break;
        case Boats::tornado:
            m_numberSize = 10;
            posY = 17;
            m_mast = QPointF(0,0);
            sailSize = 25.5;
            break;
        case Boats::startboat:
            m_numberSize = 0;
            break;
        case Boats::rib:
            m_numberSize = 10;
            posY = 10;
        default:
            break;
        }

        m_numberPath->setPos(0, posY);

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
        setOverlapLine();
        setOrder(m_order);
        update();
    }
}

void BoatGraphicsItem::deleteItem(BoatModel *boat) {
    if (boat == m_boat) {
        if (debugLevel & 1 << VIEW) std::cout << "deleting boatGraphics for model" << m_boat << std::endl;
        scene()->removeItem(this);
        delete this;
    }
}

void BoatGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    static_cast<SituationScene*>(scene())->setModelPressed(m_boat);
    bool multiSelect = (event->modifiers() & Qt::ControlModifier) != 0;
    if (!multiSelect) {
        scene()->clearSelection();
    }
    setSelected(true);
    update();
}

void BoatGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

void BoatGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

QRectF BoatGraphicsItem::boundingRect() const {
    switch (m_series) {
    case Boats::keelboat:
        return QRectF(-67, -54, 133, 108);
    case Boats::laser:
        return QRectF(-27, -22, 54, 44);
    case Boats::optimist:
        return QRectF(-15, -12, 31, 25);
    case Boats::tornado:
        return QRectF(-40, -33, 81, 66);
    case Boats::startboat:
        return QRectF(-55, -55, 110, 110);
    case Boats::rib:
        return QRectF(-35, -35, 70, 70);
    default:
        return QRectF(-50, -50, 100, 100);
    }
}

QPainterPath BoatGraphicsItem::shape() const {
    QPainterPath path;
    path.addRegion(boundingRegion(QTransform()));
    return path;
}

void BoatGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,

                             QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPainterPath path;

    if (isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);
    painter->setBrush(m_color);

    switch (m_series) {
    case Boats::keelboat:
        path.moveTo(0,-50);
        path.cubicTo(20, 0, 18, 13, 10, 50);
        path.lineTo(-10, 50);
        path.cubicTo(-18, 13, -20, 0, 0, -50);
        break;
    case Boats::laser:
        path.moveTo(0,-20);
        path.cubicTo(0.3, -19.7, 0.3, -20.0, 0.7, -19.7);
        path.cubicTo(3.3, -14.3, 6.7, -3.3, 6.7, 4.7);
        path.cubicTo(6.7, 11.0, 6.7, 14.3, 5.0, 20.0);
        path.lineTo(-5.0, 20.0);
        path.cubicTo(-6.7, 14.3, -6.7, 11.0, -6.7, 4.7);
        path.cubicTo(-6.7, -3.3, -3.3, -14.3, -0.7, -19.7);
        path.cubicTo(-0.3, -20.0, -0.3, -19.7, 0, -20);
        break;
    case Boats::optimist:
        path.moveTo(0,-11.5);
        path.cubicTo(1.5, -11.5, 1.7, -11.3, 2.9, -11.1);
        path.cubicTo(3.6, -9.4, 5.6, -4.0, 5.6, 1.5);
        path.cubicTo(5.6, 5.4, 5.0, 9.0, 4.6, 11.5);
        path.lineTo(-4.6, 11.5);
        path.cubicTo(-5.0, 9.0, -5.6, 5.4, -5.6, 1.5);
        path.cubicTo(-5.6, -4.0, -3.6, -9.4, -2.9, -11.1);
        path.cubicTo(-1.7, -11.3, -1.5, -11.5, 0, -11.5);
        break;
    case Boats::tornado:
        path.moveTo(0,0);
        path.lineTo(10.7, 0);
        path.cubicTo(11.2, -11.7, 12.2, -19.8, 13.2, -30.5);
        path.cubicTo(14.7, -20.3, 15.3, -6.1, 15.3, 6.1);
        path.cubicTo(15.3, 13.7, 14.7, 20.8, 14.7, 30.0);
        path.cubicTo(13.7, 30.5, 13.7, 30.5, 12.7, 30.5);
        path.cubicTo(12.2, 30.5, 12.2, 30.5, 10.7, 30.0);
        path.lineTo(10.7, 23.9);

        path.lineTo(-10.7, 23.9);
        path.lineTo(-10.7, 30.0);
        path.cubicTo(-12.2, 30.5, -12.2, 30.5, -12.7, 30.5);
        path.cubicTo(-13.7, 30.5, -13.7, 30.5, -14.7, 30);
        path.cubicTo(-14.7, 20.8, -15.3, 13.7, -15.3, 6.1);
        path.cubicTo(-15.3, -6.1, -14.7, -20.3, -13.2, -30.5);
        path.cubicTo(-12.2, -19.8, -11.2, -11.7, -10.7, 0);
        path.lineTo(0, 0);
        break;
    case Boats::startboat:
        path.moveTo(0,-50);
        path.cubicTo(30, -20, 20, 30, 17, 50);
        path.lineTo(-17, 50);
        path.cubicTo(-20, 30, -30, -20, 0, -50);
        path.addEllipse(-1, -10, 2, 2);
        break;
    case Boats::rib:
        path.moveTo(0,-30);
        path.cubicTo(6, -26, 12.1, -22.9, 12.4, -10.3);
        path.lineTo(12.4, 23.5);
        path.lineTo(8.9, 30);
        path.lineTo(5.5, 23.5);
        path.lineTo(-5.5, 23.5);
        path.lineTo(-8.9, 30);
        path.lineTo(-12.4, 23.5);
        path.lineTo(-12.4, -10.3);
        path.cubicTo(-12.1, -22.9, -6, -26, 0, -30);
        break;
    default:
        break;
    }

    painter->drawPath(path);
}

int BoatGraphicsItem::type() const {
    return BOAT_TYPE;
}
