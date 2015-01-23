//
// C++ Implementation: PointGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2009-2011 Thibaut GRIDEL
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
#include "point.h"

#include "commontypes.h"
#include "situationscene.h"
#include "situationmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"

#include <QPainter>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <iostream>

extern int debugLevel;
const int size = 3;


PointGraphicsItem::PointGraphicsItem(PointModel *point, QGraphicsItem *parent)
        : QGraphicsItem(parent),
        m_point(point),
        m_bubble(new BubbleGraphicsItem(m_point, this)),
        m_selected(false),
        m_laylines(new LaylinesGraphicsItem(m_point, this)) {
    setZValue(1);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    setPos(point->position());

    m_bubble->setZValue(1);

    m_points  << QPoint(0, -size)
            << QPoint(size, 0)
            << QPoint(0, size)
            << QPoint(-size, 0)
            << QPoint(0, -size);

    connect(point, SIGNAL(positionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
    connect(point->polyLine()->situation(), SIGNAL(pointRemoved(PointModel *)),
            this, SLOT(deleteItem(PointModel*)));
}


PointGraphicsItem::~PointGraphicsItem() {}

void PointGraphicsItem::setPosition(QPointF position) {
    if (pos() != position) {
        setPos(position);
        update();
    }
}

void PointGraphicsItem::deleteItem(PointModel *point) {
    if (point == m_point) {
        if (debugLevel & 1 << VIEW) std::cout << "deleting pointGraphics for model" << m_point << std::endl;
        scene()->removeItem(this);
        delete this;
    }
}

void PointGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    static_cast<SituationScene*>(scene())->setModelPressed(m_point);

    bool selection = true;
    if ((event->modifiers() & Qt::ControlModifier) != 0) {
        selection = !isSelected();
    } else {
        scene()->clearSelection();
    }
    QGraphicsItem::setSelected(selection);
}

void PointGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

void PointGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
}

QRectF PointGraphicsItem::boundingRect() const {
    return QRectF(-size, -size, 2*size, 2*size);
}

QPainterPath PointGraphicsItem::shape() const {
    QPainterPath path;
    path.addPolygon(m_points);
    return path;
}

void PointGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,

                             QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);

    painter->setBrush(Qt::black);

    painter->drawPolygon(m_points);
}

int PointGraphicsItem::type() const {
    return POINT_TYPE;
}
