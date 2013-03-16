//
// C++ Implementation: BoatGraphicsItem
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
        m_sail(new SailGraphicsItem(boat, this)),
        m_jib(new SailGraphicsItem(boat, this)),
        m_spin(new SpinnakerGraphicsItem(boat, this)),
        m_genn(new GennakerGraphicsItem(boat, this)),
        m_hasSpin(false),
        m_hasGenn(false),
        m_overlap(Boats::none),
        m_overlapLine(new QGraphicsLineItem(this)),
        m_color(boat->track()->color()),
        m_flag(Boats::noFlag),
        m_flagRect(new FlagGraphicsItem(this)),
        m_hidden(false),
        m_bubble(new BubbleGraphicsItem(m_boat, this)),
        m_series(Boats::unknown),
        m_selected(false),
        m_order(0),
        m_numberPath(new QGraphicsPathItem(this)),
        m_laylines(new LaylinesGraphicsItem(boat, this)) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    setBoundingRegionGranularity(1);

    m_numberPath->setZValue(1);
    m_flagRect->setZValue(2);
    m_sail->setZValue(3);
    m_jib->setZValue(4);
    m_spin->setZValue(5);
    m_genn->setZValue(5); // Make this same as for spinnaker on basis that a boat will have one or other of these, not both
    m_bubble->setZValue(6);

    m_numberPath->setBrush(QBrush(Qt::black));

    QPen dashPen(Qt::CustomDashLine);
    QVector<qreal> dashes;
    dashes << 5 << 5;
    dashPen.setDashPattern(dashes);
    m_overlapLine->setPen(dashPen);

    setSeries(boat->track()->series());
    setHeading(boat->heading());
    setPos(boat->position());
    setOrder(boat->order());
    setDim(boat->dim());
    m_sail->setSailAngle(m_boat->sailAngle() + m_boat->trim());
    m_jib->setSailAngle(m_boat->jibAngle() + m_boat->jibTrim());
    m_spin->setHeading(m_boat->heading());
    m_spin->setSailAngle(m_boat->spinAngle() + m_boat->spinTrim());
    m_genn->setSailAngle(m_boat->gennAngle() + m_boat->spinTrim()); // Use spinTrim() for both spinnaker and gennaker
    setSpin(boat->spin());
    setOverlap(boat->overlap());
    setDisplayFlag(boat->flag());
    setHidden(boat->hidden());

    connect(boat, SIGNAL(headingChanged(qreal)),
            this, SLOT(setHeading(qreal)));
    connect(boat, SIGNAL(headingChanged(qreal)),
            m_spin, SLOT(setHeading(qreal)));
    connect(boat, SIGNAL(positionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
    connect(boat, SIGNAL(trimmedSailAngleChanged(qreal)),
            m_sail, SLOT(setSailAngle(qreal)));
    connect(boat, SIGNAL(trimmedJibAngleChanged(qreal)),
            m_jib, SLOT(setSailAngle(qreal)));
    connect(boat, SIGNAL(spinChanged(bool)),
            this, SLOT(setSpin(bool)));
    connect(boat, SIGNAL(trimmedSpinAngleChanged(qreal)),
            m_spin, SLOT(setSailAngle(qreal)));
    connect(boat, SIGNAL(trimmedGennAngleChanged(qreal)),
            m_genn, SLOT(setSailAngle(qreal)));
    connect(boat, SIGNAL(orderChanged(int)),
            this, SLOT(setOrder(int)));
    connect(boat, SIGNAL(overlapChanged(Boats::Overlaps)),
            this, SLOT(setOverlap(Boats::Overlaps)));
    connect(boat, SIGNAL(flagChanged(Boats::Flag)),
            this, SLOT(setDisplayFlag(Boats::Flag)));
    connect(boat, SIGNAL(hiddenChanged(bool)),
            this, SLOT(setHidden(bool)));
    connect(boat, SIGNAL(dimChanged(int)),
            this, SLOT(setDim(int)));
    connect(boat->track(), SIGNAL(colorChanged(QColor)),
            this, SLOT(setColor(QColor)));
    connect(boat->track(), SIGNAL(seriesChanged(Boats::Series)),
            this, SLOT(setSeries(Boats::Series)));
    connect(boat->situation(), SIGNAL(boatRemoved(BoatModel*)),
            this, SLOT(deleteItem(BoatModel*)));
    connect(boat->track(), SIGNAL(trackSelected(bool)),
            this, SLOT(setSelected(bool)));
}


BoatGraphicsItem::~BoatGraphicsItem() {}

void BoatGraphicsItem::setHeading(qreal value) {
    if (m_angle != value) {
        m_angle = value;
        QTransform rotation;
        rotation.rotate(m_angle),
        setTransform(rotation, false);
    }
}

void BoatGraphicsItem::setPosition(QPointF position) {
    if (pos() != position) {
        setPos(position);
        update();
    }
}

void BoatGraphicsItem::setSpin(bool value) {
    m_spin->setVisible(m_hasSpin && value);
    m_genn->setVisible(m_hasGenn && value);
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
        setZValue(m_order);
    } else {
        m_numberPath->setPath(QPainterPath());
        setZValue(boat()->track()->size()+1);
    }

    update();
}

void BoatGraphicsItem::setOverlap(Boats::Overlaps value) {
    if (m_overlap != value) {
        m_overlap = value;
        setOverlapLine();
    }
}

void BoatGraphicsItem::setOverlapLine() {
    qreal size = Boats::seriesSizeList()[m_series];
    QLineF line;
    line.setP1(QPointF(m_border, size/2));
    line.setP2(QPointF(-m_border, size/2));
    if (m_overlap == Boats::none) {
        m_overlapLine->setVisible(false);
    } else {
        if (m_overlap & Boats::starboard) {
            line.setP2(QPointF(m_border + size, size/2));
        }
        if (m_overlap & Boats::port) {
            line.setP1(QPointF(-m_border - size, size/2));
        }
        m_overlapLine->setVisible(!m_hidden && true);
        m_overlapLine->setLine(line);
    }
}

void BoatGraphicsItem::setDisplayFlag(Boats::Flag value) {
    m_flag = value;
    m_flagRect->setDisplayFlag(m_flag);

    if (m_flag == Boats::noFlag) {
        m_flagRect->setVisible(false);
    } else {
        m_flagRect->setVisible(!m_hidden && true);
    }
}

void BoatGraphicsItem::setHidden(bool value) {
    prepareGeometryChange();
    m_hidden = value;
    if (m_hidden) {
        m_sail->setVisible(false);
        m_spin->setVisible(false);
        m_overlapLine->setVisible(false);
        m_flagRect->setVisible(false);
        m_bubble->setVisible(false);
        m_numberPath->setVisible(false);
        m_laylines->setVisible(false);
    } else {
        m_sail->setVisible(true);
        m_spin->setVisible(boat()->spin());
        m_overlapLine->setVisible(m_overlap != Boats::none);
        m_flagRect->setVisible(m_flag != Boats::noFlag);
        m_bubble->setVisible(!boat()->text().isEmpty());
        m_numberPath->setVisible(true);
        m_laylines->setVisible(boat()->laylines());
    }

}

void BoatGraphicsItem::setColor(QColor value) {
    if (m_color != value) {
        int alpha = m_color.alpha();
        m_color = value;
        m_color.setAlpha(alpha);
        update();
    }
}

void BoatGraphicsItem::setDim(int value) {
    m_color.setAlpha(value);
    if (value == 0 ) {
        setVisible(false);
    }
    else {
        setVisible(true);
        if (value != 255) {
            int maxSize = 0;
            foreach (const TrackModel *track, boat()->situation()->tracks()) {
                if (track->boats().size() > maxSize)
                    maxSize = track->boats().size() - 1;
            }
            setZValue(maxSize+1);
        } else {
            setZValue(boat()->order());
        }
    }
    update();
}

void BoatGraphicsItem::setVisible(bool value) {
    QGraphicsItem::setVisible(value);
}

void BoatGraphicsItem::setSeries(Boats::Series value) {
    if (m_series != value) {
        prepareGeometryChange();
        m_series = value;

        int posY = 0;
        QRectF flagRect;
        QPointF mast;
        qreal sailSize = 0;
        QPointF jibTackPos;
        qreal jibSize = 0;
        qreal spinSize = 0;
        QPointF gennTackPos;
        qreal gennPoleLength = 0;
        qreal gennSize = 0;
        qreal maxNormalSailAngle = 90;
        qreal maxNormalJibAngle = 45;
        qreal maxWithSpinSailAngle = 65;
        qreal maxWithSpinJibAngle = 35;
        QPainterPath path;
        QRectF boundingRect = QRectF(-50, -50, 100, 100);
        qreal border = 0;

        switch (m_series) {
        case Boats::keelboat:
        case Boats::keelboatwithgenn:
            m_numberSize = 12;
            posY = 25;
            flagRect = QRectF(-7.5, 30 , 15, 10);
            mast = QPointF(0, -8.7);
            sailSize = 41.5;
            jibTackPos = QPointF(0,-50);
            jibSize = 40;
            if (m_series == Boats::keelboat) {
                spinSize = 1.1 * sailSize;
            } else {
                spinSize = 0;
            }
            if (m_series == Boats::keelboatwithgenn) {
                gennTackPos = QPointF(0,-80);
                gennPoleLength = 30;
                gennSize = 70;
                maxWithSpinSailAngle = 40;
                maxWithSpinJibAngle = 35;
            } else {
                gennSize = 0;
            }
            path.moveTo(0,-50);
            path.cubicTo(20, 0, 18, 13, 10, 50);
            path.lineTo(-10, 50);
            path.cubicTo(-18, 13, -20, 0, 0, -50);
            boundingRect = QRectF(-20, -50, 40, 100);
            border = 10;
            break;
        case Boats::int49er:
            m_numberSize = 7;
            posY = 9;
            flagRect = QRectF(-3, 13, 6, 4);
            mast = QPointF(0, -2.5);
            sailSize = 27.0;
            jibTackPos = QPointF(0,-22.5);
            jibSize = 19.0;
            gennTackPos = QPointF(0,-42.5);
            gennPoleLength = 18.5;
            gennSize = 38.0;
            maxWithSpinSailAngle = 30;
            maxWithSpinJibAngle = 25;
            path.moveTo(0,-24.0);
            path.lineTo(-0.5,-24.0);
            path.cubicTo(-1.5,-23.0,-6.5,-5.5,-7.5,1.5);
            path.lineTo(-13.5,2.4);
            path.quadTo(-14.0,2.5,-14,3.0);
            path.lineTo(-14.0,24.0);
            path.lineTo(-13.5,24.0);
            path.lineTo(-12.5,21.0);
            path.lineTo(-7.5,19.0);
            path.lineTo(-5.0,22.0);
            path.lineTo(5.0,22.0);
            path.lineTo(7.5,19.0);
            path.lineTo(12.5,21.0);
            path.lineTo(13.5,24.0);
            path.lineTo(14.0,24.0);
            path.lineTo(14.0,3.0);
            path.quadTo(14.0,2.5,13.5,2.4);
            path.lineTo(7.5,1.5);
            path.cubicTo(6.5,-5.5,1.5,-23,0.5,-24.0);
            path.lineTo(0,-24.0);
            boundingRect = QRectF(-14.0, -24.0, 28.0, 48.0);
            border = 14.5;
            break;
        case Boats::int470:
            m_numberSize = 7;
            posY = 9;
            flagRect = QRectF(-3, 13, 6, 4);
            mast = QPointF(0, -7.5);
            sailSize = 26.5;
            jibTackPos = QPointF(0,-23.5);
            jibSize = 0.6*sailSize;
            spinSize = 19.0;
            path.moveTo(0,-23.5);
            path.cubicTo(0.8, -23.5, 8.5, -15.0, 8.5, 3.5);
            path.quadTo(8.5, 12.5, 5.8, 23.5);
            path.lineTo(-5.8,23.5);
            path.quadTo(-8.5, 12.5, -8.5, 3.5);
            path.cubicTo(-8.5, -15.0, -0.8, -23.5, 0, -23.5);
            boundingRect = QRectF(-8.5,-23.5,17.0,47.0);
            border = 5.8;
            break;
        case Boats::int420:
            m_numberSize = 7;
            posY = 9;
            flagRect = QRectF(-3, 13, 6, 4);
            mast = QPointF(0, -7.1);
            sailSize = 24.0;
            jibTackPos = QPointF(0,-21);
            jibSize = 0.6*sailSize;
            spinSize = 17.5;
            path.moveTo(0,-21);
            path.cubicTo(1.5, -21, 8.15, -12.5, 8.15, 3.3);
            path.quadTo(8.15, 11.5, 5.7, 20);
            path.quadTo(2.8, 21, 0, 21);
            path.quadTo(-2.8, 21, -5.7, 20);
            path.quadTo(-8.15, 11.5, -8.15, 3.3);
            path.cubicTo(-8.15, -12.5, -1.5, -21, 0, -21);
            boundingRect = QRectF(-8.15,-21,16.3,42);
            border = 5.7;
            break;
        case Boats::int29er:
            m_numberSize = 7;
            posY = 9;
            flagRect = QRectF(-3, 13, 6, 4);
            mast = QPointF(0, -3.4);
            sailSize = 20.5;
            jibTackPos = QPointF(0,-20.5);
            jibSize = 17.0;
            gennTackPos = QPointF(0,-34.2);
            gennPoleLength = 13.7;
            gennSize = 30.8;
            maxWithSpinSailAngle = 40;
            maxWithSpinJibAngle = 35;
            path.moveTo(0,-20.5);
            path.lineTo(-0.5,-20.5);
            path.cubicTo(-3.5,-10.25,-6.6,-1.1,-8.3,-0.5);
            path.lineTo(-8.8,18);
            path.lineTo(-4.4,20.5);
            path.lineTo(4.4,20.5);
            path.lineTo(8.8,18);
            path.lineTo(8.3,-0.5);
            path.cubicTo(6.6,-1.1,3.5,-10.25,0.5,-20.5);
            path.lineTo(0,-20.5);
            boundingRect = QRectF(-8.8, -20.5, 17.6, 41.0);
            border = 4.4;
            break;
        case Boats::laser:
            m_numberSize = 7;
            posY = 10;
            flagRect = QRectF(-3, 12 , 6, 4);
            mast = QPointF(0, -8.7);
            sailSize = 28.5;
            path.moveTo(0,-20);
            path.cubicTo(0.3, -19.7, 0.3, -20.0, 0.7, -19.7);
            path.cubicTo(3.3, -14.3, 6.7, -3.3, 6.7, 4.7);
            path.cubicTo(6.7, 11.0, 6.7, 14.3, 5.0, 20.0);
            path.lineTo(-5.0, 20.0);
            path.cubicTo(-6.7, 14.3, -6.7, 11.0, -6.7, 4.7);
            path.cubicTo(-6.7, -3.3, -3.3, -14.3, -0.7, -19.7);
            path.cubicTo(-0.3, -20.0, -0.3, -19.7, 0, -20);
            boundingRect = QRectF(-6.7, -20, 13.4, 40);
            border = 5;
            break;
        case Boats::firefly:
            m_numberSize = 7;
            posY = 10;
            flagRect = QRectF(-3, 12 , 6, 4);
            mast = QPointF(0, -6.5);
            sailSize = 21.8;
            jibTackPos = QPointF(0, -18.3);
            jibSize = 14;
            path.moveTo(0,-18.3);
            path.cubicTo(3.7, -14.0, 7.6, -8.0, 7.6, 1.8);
            path.cubicTo(7.6, 4.2, 7.6, 9.2, 4.9, 18.3);
            path.lineTo(-4.9, 18.3);
            path.cubicTo(-7.6, 9.2, -7.6, 4.2, -7.6, 1.8);
            path.cubicTo(-7.6, -8.0, -3.7, -14.0, 0, -18.3);
            boundingRect = QRectF(-7.6, -18.3, 15.2, 36.6);
            border = 4.9;
            break;
        case Boats::topper:
            m_numberSize = 7;
            posY = 8;
            flagRect = QRectF(-3, 10 , 6, 4);
            mast = QPointF(0, -6.5);
            sailSize = 23.5;
            path.moveTo(0,-17);
            path.cubicTo(2.4, -17.0, 2.8, -16.0, 3.7, -14.0);
            path.cubicTo(4.6, -12.0, 5.8, -6.0, 5.8, -1.0);
            path.cubicTo(5.8, 1.0, 5.8, 8, 4.4, 17.0);
            path.lineTo(-4.4, 17.0);
            path.cubicTo(-5.8, 8, -5.8, 1.0, -5.8, -1.0);
            path.cubicTo(-5.8, -6.0, -4.6, -12.0, -3.7, -14.0);
            path.cubicTo(-2.8, -16.0, -2.4, -17.0, 0, -17);
            boundingRect = QRectF(-5.8, -17, 11.6, 34);
            border = 4.4;
            break;
        case Boats::optimist:
            m_numberSize = 6;
            posY = 3;
            flagRect = QRectF(-2.25, 5 , 4.5, 3);
            mast = QPointF(0,-6.9);
            sailSize = 16.5;
            path.moveTo(0,-11.5);
            path.cubicTo(1.5, -11.5, 1.7, -11.3, 2.9, -11.1);
            path.cubicTo(3.6, -9.4, 5.6, -4.0, 5.6, 1.5);
            path.cubicTo(5.6, 5.4, 5.0, 9.0, 4.6, 11.5);
            path.lineTo(-4.6, 11.5);
            path.cubicTo(-5.0, 9.0, -5.6, 5.4, -5.6, 1.5);
            path.cubicTo(-5.6, -4.0, -3.6, -9.4, -2.9, -11.1);
            path.cubicTo(-1.7, -11.3, -1.5, -11.5, 0, -11.5);
            boundingRect = QRectF(-5.6, -11.5, 11.2, 23);
            border = 4.6;
            break;
        case Boats::tornado:
            m_numberSize = 10;
            posY = 17;
            flagRect = QRectF(-4.5, 17.5 , 9, 6);
            mast = QPointF(0,0);
            sailSize = 25.5;
            maxNormalSailAngle = 20;
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
            boundingRect = QRectF(-15.3, -30.5, 30.6, 61);
            border = 14.7;
            break;
        case Boats::startboat:
            m_numberSize = 0;
            flagRect = QRectF(-7.5, 30 , 15, 10);
            path.moveTo(0,-50);
            path.cubicTo(30, -20, 20, 30, 17, 50);
            path.lineTo(-17, 50);
            path.cubicTo(-20, 30, -30, -20, 0, -50);
            path.addEllipse(-1, -10, 2, 2);
            boundingRect = QRectF(-30, -50, 60, 100);
            border = 17;
            break;
        case Boats::rib:
            m_numberSize = 10;
            posY = 10;
            flagRect = QRectF(-5, 15 , 10, 6.7);
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
            boundingRect = QRectF(-12.4, -30, 24.8, 60);
            border = 8.9;
        default:
            break;
        }

        m_hullPath = path;
        m_boundingRect = boundingRect;

        m_numberPath->setPos(0, posY);

        m_flagRect->setRect(flagRect);

        if (sailSize) {
            m_boat->setMaxNormalSailAngle(maxNormalSailAngle);
            m_boat->setMaxWithSpinSailAngle(maxWithSpinSailAngle);
            m_sail->setPosition(mast);
            m_sail->setSailSize(sailSize);
            m_sail->setVisible(true);
        } else {
            m_sail->setVisible(false);
        }

        if (jibSize) {
            m_boat->setMaxNormalJibAngle(maxNormalJibAngle);
            m_boat->setMaxWithSpinJibAngle(maxWithSpinJibAngle);
            m_jib->setPosition(jibTackPos);
            m_jib->setSailSize(jibSize);
            m_jib->setVisible(true);
        } else {
            m_jib->setVisible(false);
        }

        m_hasSpin = false;
        m_hasGenn = false;

        if (spinSize) {
            m_hasSpin = true;
            m_spin->setPosition(mast);
            m_spin->setSailSize(spinSize);
            m_spin->setVisible(m_boat->spin());
        } else {
            m_spin->setVisible(false);
        }

        if (gennSize) {
            m_hasGenn = true;
            m_genn->setPosition(gennTackPos);
            m_genn->setPoleLength(gennPoleLength);
            m_genn->setSailSize(gennSize);
            m_genn->setVisible(m_boat->spin());
        } else {
            m_genn->setVisible(false);
        }

        m_boat->setHasSpin(m_hasSpin || m_hasGenn); // set setHasSpin true irrespective of type of spinnaker (is just used to control whether Toggle Spinnaker menu item is active)

        m_border = border;
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
    m_multiSelect = (event->modifiers() & Qt::ControlModifier) != 0;
    m_trackSelect = (event->modifiers() & Qt::ShiftModifier) != 0;
    if (!isSelected()) {
        if (!m_multiSelect) {
            scene()->clearSelection();
        }
        if (!m_trackSelect) {
            setSelected(true);
        } else {
            m_boat->track()->setSelected(true);
        }
        m_actOnMouseRelease=false;
    } else {
        m_actOnMouseRelease=true;
        if (m_trackSelect) {
            if (!m_multiSelect) {
                scene()->clearSelection();
                m_actOnMouseRelease=false;
            }
            m_boat->track()->setSelected(true); // NB In this case do NOT set m_actOnMouseRelease to false as mouse click should deselect all boats on track
        }
    }
    if ((event->button() & Qt::RightButton) != 0) {
        m_actOnMouseRelease = false;
    }
    update();
}

void BoatGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
    m_actOnMouseRelease=false;
}

void BoatGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
    if (m_actOnMouseRelease) {
        if (!m_multiSelect) {
            scene()->clearSelection();
            if (!m_trackSelect) {
                setSelected(true);
            } else {
                m_boat->track()->setSelected(true);
            }
        } else {
            if(!m_trackSelect) {
                setSelected(false);
            } else {
                m_boat->track()->setSelected(false);
            }
        }
    }
}

void BoatGraphicsItem::setSelected(bool selected) {
    QGraphicsItem::setSelected(selected);
}

QRectF BoatGraphicsItem::boundingRect() const {
    if (m_hidden)
        return QRectF(-3, -3, 6, 6);
    else
        return m_boundingRect;
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

    if (isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);
    painter->setBrush(m_color);
    if (m_hidden)
        painter->drawEllipse(-3, -3, 6, 6);
    else
        painter->drawPath(m_hullPath);

}

int BoatGraphicsItem::type() const {
    return BOAT_TYPE;
}
