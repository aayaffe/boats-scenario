//
// C++ Implementation: situationview
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
#include <iostream>

#include "commontypes.h"

#include "situationview.h"

#include <QtGui/QWheelEvent>

extern int debugLevel;

SituationView::SituationView(QWidget *parent)
    :QGraphicsView(parent) {
}

SituationView::SituationView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent),
    scaleValue(1),
    lookDirectionValue(0),
    tiltValue(0) {
    setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect( scene, SIGNAL(centerChanged(QPointF)),
             this, SLOT(setCenter(QPointF)));
    connect( scene, SIGNAL(lookDirectionChanged(qreal)),
             this, SLOT(setLookDirection(qreal)));
    connect( scene, SIGNAL(tiltChanged(qreal)),
             this, SLOT(setTilt(qreal)));
}

SituationView::~SituationView() {
}

/**
    Takes a picture of the current view and returns it as a QPixmap.
    This method creates a QPixmap and calls render() on that pixmap
    before returning it.
*/
QPixmap SituationView::screenShot() {
    QPixmap pixmap(size());
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    render(&painter);
    return pixmap;
}

/**
    Handles mouse wheel event to zoom in or out.
    This method will call setScale() with rotation side.
*/

void SituationView::wheelEvent(QWheelEvent *event) {
    if (debugLevel & 1 << VIEW) std::cout << "wheel event " << event->delta() << std::endl;
    setScale(event->delta() > 0);
}


void SituationView::mousePressEvent(QMouseEvent *event) {
    setDragMode(QGraphicsView::ScrollHandDrag);
    QGraphicsView::mousePressEvent(event);
}

void SituationView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
    setDragMode(QGraphicsView::NoDrag);
}

void SituationView::zoomIn() {
    setScale(true);
}

void SituationView::zoomOut() {
    setScale(false);
}

/**
    Sets an acceptable scale that fits the whole scenario.
    This method compares the matrix() before and after fitInView() and
    possibly adopts the scale, rounded to the nearest .05
*/

void SituationView::zoomFit() {
    QMatrix old = matrix();
    fitInView(scene()->itemsBoundingRect(),Qt::KeepAspectRatio);
    qreal s = matrix().m11();
    if (s < 10.0 && s > 0.1) {
        // adopt scaling
        scaleValue = (qRound(s * 20)) / 20.0;
    }
    // apply calculated or default scale
    QMatrix m(scaleValue, old.m21(), old.m21(), scaleValue, old.dx(), old.dy());
    setMatrix(m);
}

void SituationView::setLookDirection(qreal value) {
    lookDirectionValue = value;
    emit lookDirectionChanged(qRound(lookDirectionValue));
    transformView();
}

void SituationView::setTilt(qreal value) {
    tiltValue = value;
    emit tiltChanged(qRound(tiltValue));
    transformView();
}

void SituationView::setCenter(QPointF position) {
    centerOn(position);
}

/**
    Zoom in if \a in otherwise zoom out by .05 increments between
    0.1 and 10
*/

void SituationView::setScale(bool in) {
    if (in) {
        if (scaleValue < 10.0) {
            scaleValue *= 1.1;
        }
    } else if (scaleValue > 0.1) {
        scaleValue /= 1.1;
    }
    if (debugLevel & 1 << VIEW)
        std::cout << "new scale values " << scaleValue << std::endl;
    transformView();
}

void SituationView::transformView() {

    QTransform transform;
    transform.rotate(tiltValue, Qt::XAxis);
    transform.rotate(-lookDirectionValue, Qt::ZAxis);
    transform.scale(scaleValue, scaleValue);
    setTransform(transform, false);
}
