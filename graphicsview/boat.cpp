//
// C++ Implementation: BoatGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include "boat.h"

#include "commontypes.h"
#include "situationscene.h"
#include "model/situationmodel.h"
#include "model/trackmodel.h"
#include "model/boatmodel.h"

extern int debugLevel;

BoatGraphicsItem::BoatGraphicsItem(BoatModel *boat, QGraphicsItem *parent)
        : QGraphicsItem(parent),
        m_boat(boat),
        m_angle(boat->heading()),
        m_color(boat->track()->color()),
        m_series(boat->track()->series()),
        m_selected(false),
        m_order(boat->order()) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    setBoundingRegionGranularity(1);

    setSailAngle();
    setPos(boat->position());
    setZValue(m_order);

    connect(boat, SIGNAL(headingChanged(qreal)),
            this, SLOT(setHeading(qreal)));
    connect(boat, SIGNAL(positionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
    connect(boat, SIGNAL(orderChanged(int)),
            this, SLOT(setOrder(int)));
    connect(boat->track(), SIGNAL(colorChanged(QColor)),
            this, SLOT(setColor(QColor)));
    connect(boat->track(), SIGNAL(seriesChanged(Series)),
            this, SLOT(setSeries(Series)));
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
    }
}

void BoatGraphicsItem::setSailAngle() {
    qreal layline = m_boat->track()->situation()->laylineAngle() -10;
    if (m_angle< layline || m_angle>360-layline) {
        m_sailAngle = -m_angle;
        return;
    }
    switch (m_series) {
    case TORNADO:
        if (m_angle<180) {
            m_sailAngle = -20;
        } else {
            m_sailAngle = 20;
        }
        break;
    default:
        if (m_angle<180) {
            m_sailAngle = -m_angle/1.86 + 6;
        } else {
            m_sailAngle = 180-m_angle/1.86 + 6;
        }
        break;
    }
}

void BoatGraphicsItem::setPosition(QPointF position) {
    if (pos() != position) {
        setPos(position);
        update();
    }
}

void BoatGraphicsItem::setOrder(int value) {
    if (m_order != value) {
        m_order = value;
        setZValue(m_order);
        update();
    }
}

void BoatGraphicsItem::setColor(QColor value) {
    if (m_color != value) {
        m_color = value;
        update();
    }
}

void BoatGraphicsItem::setSeries(Series value) {
    if (m_series != value) {
        prepareGeometryChange();
        m_series = value;
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
    case KEELBOAT:
        return QRectF(-67, -54, 133, 108);
    case LASER:
        return QRectF(-27, -22, 54, 44);
    case OPTIMIST:
        return QRectF(-15, -12, 31, 25);
    case TORNADO:
        return QRectF(-40, -33, 81, 66);
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
    QPainterPath fpath;
    QFont f(painter->font());
    QString number = QString::number(m_order);

    switch (m_series) {
    case KEELBOAT:
        path.moveTo(0,-50);
        path.cubicTo(20, 0, 18, 13, 10, 48);
        path.lineTo(-10, 48);
        path.cubicTo(-18, 13, -20, 0, 0, -50);
        if (m_order) {
            f.setPointSize(12);
            QFontMetrics fm(f);
            fpath.addText(-fm.width(number)/2.0, 25 ,f,number);
        }
        break;
    case LASER:
        path.moveTo(0,-20);
        path.cubicTo(0.3, -19.7, 0.3, -20.0, 0.7, -19.7);
        path.cubicTo(3.3, -14.3, 6.7, -3.3, 6.7, 4.7);
        path.cubicTo(6.7, 11.0, 6.7, 14.3, 5.0, 20.0);
        path.lineTo(-5.0, 20.0);
        path.cubicTo(-6.7, 14.3, -6.7, 11.0, -6.7, 4.7);
        path.cubicTo(-6.7, -3.3, -3.3, -14.3, -0.7, -19.7);
        path.cubicTo(-0.3, -20.0, -0.3, -19.7, 0, -20);
        if (m_order) {
            f.setPointSize(7);
            QFontMetrics fm(f);
            fpath.addText(-fm.width(number)/2.0, 10 ,f,number);
        }
        break;
    case OPTIMIST:
        path.moveTo(0,-11.5);
        path.cubicTo(1.5, -11.5, 1.7, -11.3, 2.9, -11.1);
        path.cubicTo(3.6, -9.4, 5.6, -4.0, 5.6, 1.5);
        path.cubicTo(5.6, 5.4, 5.0, 9.0, 4.6, 11.5);
        path.lineTo(-4.6, 11.5);
        path.cubicTo(-5.0, 9.0, -5.6, 5.4, -5.6, 1.5);
        path.cubicTo(-5.6, -4.0, -3.6, -9.4, -2.9, -11.1);
        path.cubicTo(-1.7, -11.3, -1.5, -11.5, 0, -11.5);
        if (m_order) {
            f.setPointSize(6);
            QFontMetrics fm(f);
            fpath.addText(-fm.width(number)/2.0, 5 ,f,number);
        }
        break;
    case TORNADO:
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
        if (m_order) {
            f.setPointSize(10);
            QFontMetrics fm(f);
            fpath.addText(-fm.width(number)/2.0, 17 ,f,number);
        }
        break;
    default:
        break;
    }

    painter->rotate(m_angle);

    if (isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);
    painter->setBrush(m_color);
    painter->drawPath(path);

    painter->setBrush(Qt::black);
    painter->drawPath(fpath);

    qreal sailScale;
    switch (m_series) {
    case KEELBOAT:
        painter->translate(0,-8);
        sailScale = .83;
        break;
    case LASER:
        painter->translate(0, -8.7);
        sailScale = 0.57;
        break;
    case OPTIMIST:
        painter->translate(0,-6.9);
        sailScale = 0.33;
        break;
    case TORNADO:
        sailScale = 0.51;
        break;
    default:
        break;
    }
    QPainterPath sailPath(QPointF(0,0));
    qreal layline = m_boat->track()->situation()->laylineAngle() -10;
    if (m_angle< layline || m_angle>360-layline) {
        sailPath.lineTo(2 * sailScale, 10 * sailScale);
        sailPath.lineTo(-2 * sailScale, 20 * sailScale);
        sailPath.lineTo(2 * sailScale, 30 * sailScale);
        sailPath.lineTo(-2 * sailScale, 40 * sailScale);
        sailPath.lineTo(0 * sailScale, 50 * sailScale);
    } else if (m_angle<180) {
        sailPath.cubicTo(5 * sailScale, 10 * sailScale, 5 * sailScale, 40 * sailScale, 0 * sailScale, 50 * sailScale);
    } else {
        sailPath.cubicTo(-5 * sailScale, 10 * sailScale, -5 * sailScale, 40 * sailScale, 0 * sailScale, 50 * sailScale);
    }
    painter->rotate(m_sailAngle);
    painter->strokePath(sailPath,painter->pen());
}

int BoatGraphicsItem::type() const {
    return BOAT_TYPE;
}
