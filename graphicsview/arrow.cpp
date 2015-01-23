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
#include "arrow.h"

#include "commontypes.h"
#include "situationscene.h"
#include "situationmodel.h"
#include "windmodel.h"

#include <QPainter>
#include <QGraphicsScene>

#include <iostream>
#include <cmath>

extern int debugLevel;

ArrowGraphicsItem::ArrowGraphicsItem(WindModel *wind, QGraphicsItem *parent)
        : QGraphicsItem(parent),
          m_wind(wind) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    setVisible(wind->visible());
    setPosition(wind->position());

    setZValue(0);
    m_path.moveTo(-15,-15);
    m_path.lineTo(15,-15);
    m_path.lineTo(15,0);
    m_path.lineTo(25,0);
    m_path.lineTo(0,17);
    m_path.lineTo(-25,0);
    m_path.lineTo(-15,0);
    m_path.lineTo(-15,-15);

    connect(wind, SIGNAL(windVisibleChanged(bool)),
            this, SLOT(deleteItem(bool)));
    connect(wind, SIGNAL(positionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
    connect(wind, SIGNAL(directionChanged(qreal)),
            this, SLOT(setHeading(qreal)));
    connect(wind, SIGNAL(windReset()),
            this, SLOT(resetArrow()));

}


ArrowGraphicsItem::~ArrowGraphicsItem() {}

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

void ArrowGraphicsItem::deleteItem(bool visible) {
    if(!visible) {
        if (debugLevel & 1 << VIEW) std::cout << "deleting ArrowGraphicsItem for model" << m_wind << std::endl;
        scene()->removeItem(this);
        delete this;
    }
}

QRectF ArrowGraphicsItem::boundingRect() const {
    return QRectF(-25,-15,50,32);
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

    painter->setBrush(QColor(191,191,255,255));

    painter->drawPath(m_path);

    QString label = tr("Wind");
    painter->setPen(Qt::black);
    painter->drawText(QRectF(-15,-15,30,15),Qt::AlignCenter,label);
}


int ArrowGraphicsItem::type() const {
    return ARROW_TYPE;
}
