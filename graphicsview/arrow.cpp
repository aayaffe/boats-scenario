//
// C++ Implementation: ArrowGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2011 Thibaut GRIDEL
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
#include <cmath>

#include <QPainter>
#include <QGraphicsScene>


#include "arrow.h"

#include "commontypes.h"
#include "situationscene.h"
#include "situationmodel.h"
#include "windmodel.h"

extern int debugLevel;

ArrowGraphicsItem::ArrowGraphicsItem(WindModel *wind, QGraphicsItem *parent)
        : QGraphicsItem(parent),
          m_wind(wind) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

//    setBoundingRegionGranularity(1);

    setZValue(0);
    m_path.moveTo(0,30);
    m_path.lineTo(7,23);
    m_path.lineTo(2,23);
    m_path.lineTo(2,-30);
    m_path.lineTo(-2,-30);
    m_path.lineTo(-2,23);
    m_path.lineTo(-7,23);
    m_path.lineTo(0,30);

    connect(wind, SIGNAL(windVisibleChanged(bool)),
            this, SLOT(setVisible(bool)));
    connect(wind, SIGNAL(positionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
    connect(wind, SIGNAL(directionChanged(qreal)),
            this, SLOT(setHeading(qreal)));
    connect(wind, SIGNAL(windReset()),
            this, SLOT(resetArrow()));

}


ArrowGraphicsItem::~ArrowGraphicsItem() {}

void ArrowGraphicsItem::setVisible(bool visible) {
    QGraphicsItem::setVisible(visible);
}

void ArrowGraphicsItem::resetArrow() {
    setHeading(m_wind->windAt(0));
}

void ArrowGraphicsItem::setHeading(qreal value) {
    m_angle = value;
    prepareGeometryChange();
    setRotation(m_angle);
    update();
}

void ArrowGraphicsItem::setPosition(QPointF position) {
    prepareGeometryChange();
    setPos(position);
    update();
}

void ArrowGraphicsItem::deleteItem(WindModel *wind) {
    if (debugLevel & 1 << VIEW) std::cout << "deleting ArrowGraphicsItem for model" << wind << std::endl;
    scene()->removeItem(this);
    delete this;
}

QRectF ArrowGraphicsItem::boundingRect() const {
    return QRectF(-7, -30, 14, 60);
}

QPainterPath ArrowGraphicsItem::shape() const {
    return m_path;
}

void ArrowGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (isSelected())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::black);

    painter->setBrush(Qt::black);
    painter->drawPath(m_path);
}


int ArrowGraphicsItem::type() const {
    return ARROW_TYPE;
}
