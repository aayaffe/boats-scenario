//
// C++ Implementation: MarkGraphicsItem
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
#include <QGraphicsScene>

#include "mark.h"

#include "commontypes.h"
#include "situationscene.h"
#include "situationmodel.h"
#include "markmodel.h"
#include "bubble.h"

extern int debugLevel;

MarkGraphicsItem::MarkGraphicsItem(MarkModel *mark, QGraphicsItem *parent)
        : QGraphicsItem(parent),
        m_mark(mark),
        m_color(mark->color()),
        m_zone(mark->zone()),
        m_length(mark->length()),
        m_boatLength(m_mark->situation()->sizeForSeries(m_mark->situation()->situationSeries())),
        m_bubble(new BubbleGraphicsItem(m_mark, this)),
        m_selected(false),
        m_order(mark->order()),
        m_laylines(new LaylinesGraphicsItem(m_mark, this)) {
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
    int boatLength = m_mark->situation()->sizeForSeries((Boats::Series)value);
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
    bool multiSelect = (event->modifiers() & Qt::ControlModifier) != 0;
    if (!multiSelect) {
        scene()->clearSelection();
    }
    setSelected(true);
    update();
}

void MarkGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

void MarkGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

QRectF MarkGraphicsItem::boundingRect() const {
    int r = m_length * m_boatLength;
    return QRectF(-r, -r, 2*r, 2*r);
}

QPainterPath MarkGraphicsItem::shape() const {
    QPainterPath path;
    path.addEllipse(QPointF(0,0),10,10);
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
    painter->drawText(QRectF(-10,-10,20,20),Qt::AlignCenter,QString::number(m_order));
    if (m_zone) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(Qt::DashLine);
        int r = m_length * m_boatLength;
        painter->drawEllipse(point, r, r);
    }
}

int MarkGraphicsItem::type() const {
    return MARK_TYPE;
}
