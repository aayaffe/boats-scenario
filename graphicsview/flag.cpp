//
// C++ Implementation: FlagGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2009 Thibaut GRIDEL
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

#include "flag.h"

#include "commontypes.h"

#include <QPainter>

FlagGraphicsItem::FlagGraphicsItem(QGraphicsItem *parent)
        : QGraphicsItem(parent) {
}


FlagGraphicsItem::~FlagGraphicsItem() {}

QRectF FlagGraphicsItem::boundingRect() const {
    return m_rect;
}

QPainterPath FlagGraphicsItem::shape() const {
    QPainterPath path;
    path.addRect(m_rect);
    return path;
}

void FlagGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,

                             QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    switch (m_flag) {
        case Boats::Y: {
            painter->setBrush(Qt::red);
            painter->drawRect(m_rect);

            painter->translate(m_rect.left(), m_rect.top());
            QPolygonF polygon;
            qreal sixth = m_rect.width() / 6;
            qreal height = m_rect.height();
            polygon << QPointF(0, 0) << QPointF(sixth, 0) << QPointF(-3*sixth, height) << QPointF(-4*sixth, height);
            painter->setPen(Qt::transparent);
            painter->setBrush(Qt::yellow);
            for (int i=0; i<5; i++) {
                painter->drawPolygon(polygon.intersected(QPolygonF(QRectF(0, 0, m_rect.width(), m_rect.height()))));
                polygon.translate(2*sixth, 0);
            }
            break;
        }
        case Boats::Red:
            painter->setBrush(Qt::red);
            painter->drawRect(m_rect);
            break;
        case Boats::Yellow:
            painter->setBrush(Qt::yellow);
            painter->drawRect(m_rect);
            break;
        case Boats::Blue:
            painter->setBrush(Qt::blue);
            painter->drawRect(m_rect);
            break;
        case Boats::Green: {
            painter->setBrush(Qt::white);
            painter->drawRect(m_rect);

            painter->translate(m_rect.left(), m_rect.top());
            QPolygonF polygon;
            qreal halfw = m_rect.width() / 2;
            qreal halfh = m_rect.height() / 2;
            polygon << QPointF(0, 0) << QPointF(halfw, 0) << QPointF(halfw, halfh) << QPointF(0, halfh);
            painter->setBrush(Qt::green);
            painter->drawPolygon(polygon);
            polygon.translate(halfw, halfh);
            painter->drawPolygon(polygon);
            break;
            }
        default:
            break;
    }
}
