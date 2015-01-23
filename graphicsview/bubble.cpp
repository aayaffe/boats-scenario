//
// C++ Implementation: BubbleGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2010 Thibaut GRIDEL
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
#include "bubble.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"

#include "undocommands.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include <iostream>

extern int debugLevel;

BubbleGraphicsItem::BubbleGraphicsItem(PositionModel *model, QGraphicsItem *parent)
        : QGraphicsTextItem(parent),
        m_model(model),
        m_tail(new QGraphicsPolygonItem(parent)) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    m_tail->setZValue(0);
    m_tail->setBrush(Qt::white);

    updateText(model->text());
    setPosition(model->textPosition());

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(setText()));


    if (m_model->inherits("BoatModel")) {
        BoatModel *boat = static_cast<BoatModel*> (m_model);
        connect(boat, SIGNAL(headingChanged(qreal)),
                this, SLOT(setTail()));
    }
    connect(model, SIGNAL(textChanged(QString)),
            this, SLOT(updateText(QString)));
    connect(model, SIGNAL(textPositionChanged(QPointF)),
            this, SLOT(setPosition(QPointF)));
}


BubbleGraphicsItem::~BubbleGraphicsItem() {}

/// update view from model changes
void BubbleGraphicsItem::updateText(QString value) {
    setVisible(!value.isEmpty());
    if (document()->toPlainText() != value)
        setPlainText(value);
    setTail();
}

/// set model from control changes
void BubbleGraphicsItem::setText() {
    if (m_model->situation()) {
        if (document()->toPlainText() != m_model->text()) {
            m_model->situation()->undoStack()->push(new SetTextUndoCommand(m_model, document()->toPlainText()));
            setVisible(!document()->toPlainText().isEmpty());
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
    //centerx,y is the centre of the text relative to pos()
    double centerx = boundingRect().width()/2 - 4;
    double centery = boundingRect().height()/2 - 4;

    double a = 0;
    if (BoatModel *boat = dynamic_cast<BoatModel*> (m_model)) {
        a = -boat->heading();
        // set the transform to have horizontal text
        setTransformOriginPoint(centerx, centery); // Rotate about centre of text rather than top left corner
        setRotation(a);
    }

    // x,y is the center of the text
    double x = pos().x() + centerx;
    double y = pos().y() + centery;
    // t is a factor for the width of the tail
    double t = boundingRect().height()/5;
    // q is the angle of the tail
    double q = atan2(y, x);
    QPolygonF polygon;
    polygon << QPointF(x,y) << QPointF(x - t*sin(q), y + t*cos(q))
            << QPointF(x/4, y/4) << QPointF(x + t*sin(q), y - t*cos(q));
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

void BubbleGraphicsItem::setVisible(bool visible) {
    QGraphicsItem::setVisible(visible);
    m_tail->setVisible(visible);
}


void BubbleGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    m_clickTime.start();
    QGraphicsTextItem::mousePressEvent(event);
    m_fromPosition = m_model->textPosition();
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
        m_model->situation()->undoStack()->push(new MoveTextUndoCommand(m_model, pos() - m_fromPosition));
    }
}

void BubbleGraphicsItem::focusOutEvent(QFocusEvent *event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    QGraphicsTextItem::focusOutEvent(event);
}
