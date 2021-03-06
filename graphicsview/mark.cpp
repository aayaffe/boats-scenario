//
// C++ Implementation: MarkGraphicsItem
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
#include "mark.h"

#include "commontypes.h"
#include "situationscene.h"
#include "situationmodel.h"
#include "markmodel.h"
#include "bubble.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <iostream>

extern int debugLevel;

MarkGraphicsItem::MarkGraphicsItem(MarkModel *mark, QGraphicsItem *parent)
        : QGraphicsItem(parent),
        m_mark(mark),
        m_color(mark->color()),
        m_zone(mark->zone()),
        m_length(mark->length()),
        m_boatLength(Boats::seriesSizeList()[m_mark->situation()->situationSeries()]),
        m_bubble(new BubbleGraphicsItem(m_mark, this)),
        m_order(mark->order()),
        m_laylines(new LaylinesGraphicsItem(m_mark, this)),
        m_heading(mark->heading()),
        m_arrowVisible(mark->arrowVisible()),
        m_leaveToPort(mark->leaveToPort()),
        m_labelVisible(mark->labelVisible()),
        m_labelText(mark->labelText()) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    setPos(mark->position());
    setZValue(m_order);

    m_bubble->setZValue(1);

    connect(mark, SIGNAL(positionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
    connect(mark, SIGNAL(orderChanged(int)),
            this, SLOT(setOrder(int)));
    connect(mark, SIGNAL(colorChanged(QColor)),
            this, SLOT(setColor(QColor)));
    connect(mark, SIGNAL(zoneChanged(bool)),
            this, SLOT(setZone(bool)));
    connect(mark, SIGNAL(lengthChanged(int)),
            this, SLOT(setLength(int)));
    connect(mark->situation(), SIGNAL(seriesChanged(int)),
            this, SLOT(setSeries(int)));
    connect(mark->situation(), SIGNAL(markRemoved(MarkModel*)),
            this, SLOT(deleteItem(MarkModel*)));
    connect(mark, SIGNAL(headingChanged(qreal)),
            this, SLOT(setHeading(qreal)));
    connect(mark, SIGNAL(arrowVisibilityChanged(bool)),
            this, SLOT(setArrowVisible(bool)));
    connect(mark, SIGNAL(leaveToPortChanged(bool)),
            this, SLOT(setLeaveToPort(bool)));
    connect(mark, SIGNAL(labelVisibilityChanged(bool)),
            this, SLOT(setLabelVisible(bool)));
    connect(mark, SIGNAL(labelTextChanged(QString)),
            this, SLOT(setLabelText(QString)));
}


MarkGraphicsItem::~MarkGraphicsItem() {}

void MarkGraphicsItem::setPosition(QPointF position) {
    if (pos() != position) {
        setPos(position);
        update();
    }
}

void MarkGraphicsItem::setOrder(int value) {
    if (m_order != value) {
        m_order = value;
        update();
    }
}

void MarkGraphicsItem::setColor(QColor value) {
    if (m_color != value) {
        m_color = value;
        update();
    }
}

void MarkGraphicsItem::setZone(bool value) {
    if (m_zone != value) {
        prepareGeometryChange();
        m_zone = value;
        update();
    }
}

void MarkGraphicsItem::setLength(int value) {
    if (m_length != value) {
        prepareGeometryChange();
        m_length = value;
        update();
    }
}

void MarkGraphicsItem::setSeries(int value) {
    int boatLength = Boats::seriesSizeList()[value];
    if (m_boatLength != boatLength) {
        prepareGeometryChange();
        m_boatLength = boatLength;
        update();
    }
}

void MarkGraphicsItem::deleteItem(MarkModel *mark) {
    if (mark == m_mark) {
        if (debugLevel & 1 << VIEW) std::cout << "deleting markGraphics for model" << m_mark << std::endl;
        scene()->removeItem(this);
        delete this;
    }
}

void MarkGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    m_multiSelect = (event->modifiers() & Qt::ControlModifier) != 0;

    bool selection = true;
    if (m_multiSelect) {
        selection = !isSelected();
    }

    setSelected(selection);
    if (selection) {
        m_mark->situation()->addSelectedMark(m_mark);
    } else {
        m_mark->situation()->removeSelectedModel(m_mark);
    }

    update();
}

void MarkGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

void MarkGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

void MarkGraphicsItem::setHeading(qreal heading) {
    if (m_heading != heading) {
        m_heading = heading;
        QTransform rotation;
        rotation.rotate(m_heading),
        setTransform(rotation, false);
    }
}

void MarkGraphicsItem::setArrowVisible(bool visible) {
    if (m_arrowVisible != visible) {
        m_arrowVisible = visible;
        update();
    }
}

void MarkGraphicsItem::setLeaveToPort(bool leaveToPort) {
    if (m_leaveToPort != leaveToPort) {
        m_leaveToPort = leaveToPort;
        update();
    }
}

void MarkGraphicsItem::setLabelVisible(bool visible) {
    if (m_labelVisible != visible) {
        m_labelVisible = visible;
        update();
    }
}

void MarkGraphicsItem::setLabelText(QString text) {
    if (m_labelText != text) {
        m_labelText = text;
        update();
    }
}

QRectF MarkGraphicsItem::boundingRect() const {
    int r = m_length * m_boatLength;
    return QRectF(-r, -r, 2*r, 2*r);
}

QPainterPath MarkGraphicsItem::shape() const {
    QPainterPath path;
// Need to increase size of shape to include mark arrow so that can click on it to rotate
    path.addEllipse(QPointF(0,0),35,35);
    return path;
}

void MarkGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,

                             QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);

    painter->setBrush(m_color);
    QPointF point(0, 0);
    painter->drawEllipse(point,10,10);
    if (m_labelVisible) {
        painter->rotate(-m_heading);
        painter->drawText(QRectF(-35,-35,70,70),Qt::AlignCenter,m_labelText);
        painter->rotate(m_heading);
    }
    if (m_zone) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(Qt::DashLine);
        int r = m_length * m_boatLength;
        painter->drawEllipse(point, r, r);
    }
//  Additional code to draw mark rounding arrow
    if (m_arrowVisible) {
        QPainterPath port_path, stbd_path;

        port_path.moveTo(-25,0);
        port_path.lineTo(-20,0);
        port_path.lineTo(-30,10);
        port_path.lineTo(-40,0);
        port_path.lineTo(-35,0);
        port_path.arcTo(-35,-35,70,70,180,-90);
        port_path.arcTo(-25,-25,50,50,90,90);

        stbd_path.moveTo(0,-25);
        stbd_path.lineTo(0,-20);
        stbd_path.lineTo(10,-30);
        stbd_path.lineTo(0,-40);
        stbd_path.lineTo(0,-35);
        stbd_path.arcTo(-35,-35,70,70,90,90);
        stbd_path.arcTo(-25,-25,50,50,180,-90);

        QPen arrowpen;
        arrowpen.setWidth(2);
        arrowpen.setColor(Qt::lightGray);
        painter->setPen(arrowpen);
        painter->setBrush(Qt::NoBrush);

        painter->rotate(45.0);
        if (m_leaveToPort) {
            painter->drawPath(port_path);
        }
        else {
            painter->drawPath(stbd_path);
        }
    }
}

int MarkGraphicsItem::type() const {
    return MARK_TYPE;
}
