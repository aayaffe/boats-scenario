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
        m_spin(new SpinnakerGraphicsItem(boat, this)),
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
    m_spin->setZValue(4);
    m_bubble->setZValue(5);

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
    m_sail->setSailAngle(m_boat->trimmedSailAngle());
    m_spin->setHeading(m_boat->heading());
    m_spin->setSailAngle(m_boat->spinAngle() + m_boat->spinTrim());
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
    connect(boat, SIGNAL(spinChanged(bool)),
            this, SLOT(setSpin(bool)));
    connect(boat, SIGNAL(trimmedSpinAngleChanged(qreal)),
            m_spin, SLOT(setSailAngle(qreal)));
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
    m_spin->setVisible(!m_hidden && value);
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
    qreal size = m_boat->situation()->sizeForSeries(m_series);
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
        QPainterPath path;

        switch (m_series) {
        case Boats::keelboat:
            m_numberSize = 12;
            posY = 25;
            flagRect = QRectF(-7.5, 30 , 15, 10);
            mast = QPointF(0, -8.7);
            sailSize = 41.5;
            path.moveTo(0,-50);
            path.cubicTo(20, 0, 18, 13, 10, 50);
            path.lineTo(-10, 50);
            path.cubicTo(-18, 13, -20, 0, 0, -50);
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
            break;
        case Boats::tornado:
            m_numberSize = 10;
            posY = 17;
            flagRect = QRectF(-4.5, 17.5 , 9, 6);
            mast = QPointF(0,0);
            sailSize = 25.5;
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
            m_numberSize = 0;
            flagRect = QRectF(-7.5, 30 , 15, 10);
            path.moveTo(0,-50);
            path.cubicTo(30, -20, 20, 30, 17, 50);
            path.lineTo(-17, 50);
            path.cubicTo(-20, 30, -30, -20, 0, -50);
            path.addEllipse(-1, -10, 2, 2);
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
        default:
            break;
        }

        m_hullPath = path;
        m_numberPath->setPos(0, posY);

        m_flagRect->setRect(flagRect);

        m_sail->setPosition(mast);
        m_sail->setSailSize(sailSize);
        if (m_series == Boats::keelboat) {
            m_spin->setSailSize(1.1*sailSize);
            m_spin->setPosition(mast);
        } else {
            m_spin->setSailSize(0);
        }
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
    if (m_hidden)
        return QRectF(-3, -3, 6, 6);
    else
        return m_hullPath.boundingRect();
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
