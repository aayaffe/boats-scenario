//
// C++ Implementation: BubbleGraphicsItem
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

#include "bubble.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"

#include "undocommands.h"

extern int debugLevel;

BubbleGraphicsItem::BubbleGraphicsItem(BoatModel *boat, QGraphicsItem *parent)
        : QGraphicsTextItem(parent),
        m_boat(boat),
        m_tail(new QGraphicsPolygonItem(parent)) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    m_tail->setZValue(0);
    m_tail->setBrush(Qt::white);

    updateText(boat->text());
    setPosition(boat->textPosition());

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(setText()));

    connect(boat, SIGNAL(headingChanged(qreal)),
            this, SLOT(setTail()));
    connect(boat, SIGNAL(textChanged(QString)),
            this, SLOT(updateText(QString)));
    connect(boat, SIGNAL(textPositionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
}


BubbleGraphicsItem::~BubbleGraphicsItem() {}

/// update view from model changes
void BubbleGraphicsItem::updateText(QString value) {
    setVisible(!value.isEmpty());
    m_tail->setVisible(!value.isEmpty());
    if (document()->toPlainText() != value)
        setPlainText(value);
    setTail();
}

/// set model from control changes
void BubbleGraphicsItem::setText() {
    if (m_boat->track()->situation()) {
        if (document()->toPlainText() != m_boat->text()) {
            m_boat->track()->situation()->undoStack()->push(new SetTextUndoCommand(m_boat, document()->toPlainText()));
            setVisible(!document()->toPlainText().isEmpty());
            m_tail->setVisible(!document()->toPlainText().isEmpty());
            setTail();
        }
    }
}

/// update view from model changes
void BubbleGraphicsItem::setPosition(QPointF position) {
    setPos(position);
    setTail();
    update();
}

void BubbleGraphicsItem::setTail() {
    double a = -boat()->heading() * M_PI / 180;
    // set the transform to have horizontal text
    setTransform(QTransform().rotateRadians(a));

    //centerx,y is half the size of the text
    double centerx = boundingRect().width()/2;
    double centery = boundingRect().height()/2;
    // x,y is the center of the text
    double x = pos().x() + centerx*cos(a) - centery*sin(a);
    double y = pos().y() + centerx*sin(a) + centery*cos(a);
    // t is a factor for the width of the tail
    double t = boundingRect().height()/5;
    // q is the angle of the tail
    double q = atan2(y, x);
    QPolygonF polygon;
    polygon << QPointF(x,y) << QPointF(x - t*sin(q), y + t*cos(q))
            << QPointF(x/2, y/2) << QPointF(x + t*sin(q), y - t*cos(q));
    m_tail->setPolygon(polygon);
}

QRectF BubbleGraphicsItem::boundingRect() const {
    return QGraphicsTextItem::boundingRect().adjusted(-4, -4, 4, 4);
}

void BubbleGraphicsItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget) {
    QPainterPath path;
    path.addRoundedRect(QGraphicsTextItem::boundingRect().adjusted(-2, -2, 2, 2),
                        15.0, 15.0, Qt::RelativeSize);
    painter->save();
    painter->setBrush(Qt::white);
    painter->drawPath(path);
    painter->restore();
    QGraphicsTextItem::paint(painter, option, widget);
}

void BubbleGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    m_clickTime.start();
    QGraphicsTextItem::mousePressEvent(event);
    m_fromPosition = m_boat->textPosition();
}

void BubbleGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsTextItem::mouseMoveEvent(event);
    setTail();
}

void BubbleGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    bool click = (m_clickTime.elapsed() < 250);
    if (click) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFocus(Qt::MouseFocusReason);
    } else {
        setTextInteractionFlags(Qt::NoTextInteraction);
    }
    QGraphicsTextItem::mouseReleaseEvent(event);
    if (pos() != m_fromPosition) {
        m_boat->track()->situation()->undoStack()->push(new MoveTextUndoCommand(m_boat, pos() - m_fromPosition));
    }
}

void BubbleGraphicsItem::focusOutEvent(QFocusEvent *event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    QGraphicsTextItem::focusOutEvent(event);
}
