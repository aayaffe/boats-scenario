//
// C++ Implementation: situationscene
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
#include "situationscene.h"

#include "commontypes.h"

#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"
#include "windmodel.h"
#include "scenarioanimation.h"
#include "trackanimation.h"
#include "statemachine.h"

#include "boat.h"
#include "track.h"
#include "mark.h"
#include "polyline.h"
#include "point.h"
#include "arrow.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

#include <iostream>
#include <cmath>

extern int debugLevel;

SituationScene::SituationScene(SituationModel *situation)
        : QGraphicsScene(situation),
        m_situation(situation),
        m_time(QTime::currentTime()),
        m_clickTime(QTime::currentTime()),
        m_clickState(SINGLE),
        m_defaultPopup(0),
        m_boatPopup(0),
        m_markPopup(0),
        m_pointPopup(0) {

    setItemIndexMethod(NoIndex);

    // set a minimum scene rect
    setSceneRect(-10000, -10000, 20000, 20000);

    // react to self change of selection
    connect(this, SIGNAL(selectionChanged()),
            this, SLOT(setSelectedModels()));

    // react to model track add
    connect(situation, SIGNAL(trackAdded(TrackModel*)),
            this, SLOT(addTrack(TrackModel*)));

    // react to model boat add
    connect(situation, SIGNAL(boatAdded(BoatModel*)),
            this, SLOT(addBoatItem(BoatModel*)));

    // react to model mark add
    connect(situation, SIGNAL(markAdded(MarkModel*)),
            this, SLOT(addMarkItem(MarkModel*)));

    // react to model line add
    connect(situation, SIGNAL(polyLineAdded(PolyLineModel*)),
            this, SLOT(addPolyLine(PolyLineModel*)));

    // react to model point add
    connect(situation, SIGNAL(pointAdded(PointModel*)),
            this, SLOT(addPoint(PointModel*)));

    // react to layline angle
    connect(situation, SIGNAL(laylineChanged(const int)),
            this, SLOT(setLaylines(const int)));

    connect(&situation->wind(), SIGNAL(windVisibleChanged(bool)),
            this, SLOT(setWind(bool)));

    connect(situation, SIGNAL(lookDirectionChanged(qreal)),
             this, SIGNAL(lookDirectionChanged(qreal)));
    connect(situation, SIGNAL(tiltChanged(qreal)),
            this, SIGNAL(tiltChanged(qreal)));

    setLaylines(situation->laylineAngle());

    connect(situation->stateMachine()->animationState(), SIGNAL(entered()),
            this, SLOT(setAnimation()));

}

void SituationScene::addTrack(TrackModel *track) {
    if (debugLevel & 1 << VIEW) std::cout << "adding track graphics for model " << track << std::endl;
    TrackGraphicsItem *trackItem = new TrackGraphicsItem(track);
    addItem(trackItem);
}

void SituationScene::addBoatItem(BoatModel *boat) {
    if (debugLevel & 1 << VIEW) std::cout << "adding boat graphics for model " << boat << std::endl;
    BoatGraphicsItem *boatItem = new BoatGraphicsItem(boat);
    addItem(boatItem);
}

void SituationScene::addMarkItem(MarkModel *mark) {
    if (debugLevel & 1 << VIEW) std::cout << "adding mark graphics for model " << mark << std::endl;
    MarkGraphicsItem *markItem = new MarkGraphicsItem(mark);
    addItem(markItem);
}

void SituationScene::addPolyLine(PolyLineModel *polyline) {
    if (debugLevel & 1 << VIEW) std::cout << "adding line graphics for model " << polyline << std::endl;
    PolyLineGraphicsItem *polyLineItem = new PolyLineGraphicsItem(polyline);
    addItem(polyLineItem);
}

void SituationScene::addPoint(PointModel *point) {
    if (debugLevel & 1 << VIEW) std::cout << "adding point graphics for model " << point << std::endl;
    PointGraphicsItem *pointItem = new PointGraphicsItem(point);
    addItem(pointItem);
}

void SituationScene::setWind(bool visible) {
    if (visible) {
        if (debugLevel & 1 << VIEW) std::cout << "adding wind graphics" << std::endl;
        ArrowGraphicsItem *arrow= new ArrowGraphicsItem(&m_situation->wind());
        addItem(arrow);
    }
}

/**
    Creates a BoatGraphicsItem for animation purpose
*/

void SituationScene::setAnimation() {

    clearSelection();
    foreach (TrackAnimation *track, m_situation->animation()->animationItems()) {
        BoatGraphicsItem *boatItem = new BoatGraphicsItem(track->boat());
        boatItem->setOrder(0);
        addItem(boatItem);
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
        shadow->setXOffset(4);
        shadow->setYOffset(4);
        shadow->setBlurRadius(4);
        boatItem->setGraphicsEffect(shadow);

        if(track->boat()->track()->followTrack()) {
            connect(boatItem->boat(), SIGNAL(positionChanged(QPointF)),
                    this, SIGNAL(centerChanged(QPointF)));
            disconnect(m_situation, SIGNAL(lookDirectionChanged(qreal)));
            connect(boatItem->boat(), SIGNAL(headingChanged(qreal)),
                    this, SIGNAL(lookDirectionChanged(qreal)));
        }
    }

}

/**
    Reacts to user keyboard actions in the Scene
    This method modifies the associated SituationModel.
    Handled keys are for
    - heading of selected boats (+,-)
    - position of selected objects (H (left), L (right), J (down), K (up))
*/

void SituationScene::keyPressEvent(QKeyEvent *event) {

    if (!m_situation->selectedModels().isEmpty()) {
        if (event->key() == Qt::Key_Left) {
            m_situation->setCurPosition(m_situation->curPosition() + QPointF(-5,0));
            m_situation->stateMachine()->lmbMove();

        } else if (event->key() == Qt::Key_Right) {
            m_situation->setCurPosition(m_situation->curPosition() + QPointF(5,0));
            m_situation->stateMachine()->lmbMove();

        } else if (event->key() == Qt::Key_Up) {
            m_situation->setCurPosition(m_situation->curPosition() + QPointF(0,-5));
            m_situation->stateMachine()->lmbMove();

        } else if (event->key() == Qt::Key_Down) {
            m_situation->setCurPosition(m_situation->curPosition() + QPointF(0,5));
            m_situation->stateMachine()->lmbMove();

        } else if (event->key() == Qt::Key_Plus) {
            m_situation->rotateModel(5.0);

        } else if (event->key() == Qt::Key_Minus) {
            m_situation->rotateModel(-5.0);

        } else {
            QGraphicsScene::keyPressEvent(event);
        }
    } else {
        QGraphicsScene::keyPressEvent(event);
    }
}

void SituationScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (debugLevel & 1 << VIEW) std::cout << "Mouse Press " << m_situation->selectedModels().size() << std::endl;

    // if we don't use Ctrl, then clear selection
    if ((event->modifiers() & Qt::ControlModifier) == 0) {
        m_situation->clearSelectedModels();
        clearSelection();
    }

    QGraphicsScene::mousePressEvent(event);
    m_clickTime.start();
}

/**
    Handles a timer to limit user interaction
    This method limits the move processing to one event handled per 40ms.
    This is very noticeable on Windows platform, and the
    culprit is the drawing, not the model setting.
*/

void SituationScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    // limit update rate to 40ms
    if ((m_time.elapsed() <40) ) {
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
    m_situation->setCurPosition(event->scenePos());
    if (event->buttons() == Qt::LeftButton
        && (event->modifiers() & Qt::MetaModifier) == 0) {
        m_situation->stateMachine()->lmbMove();
    }

    if (event->buttons() == Qt::RightButton
        || (event->buttons() == Qt::LeftButton
            && ((event->modifiers() & Qt::MetaModifier) != 0))) {
        m_situation->stateMachine()->rmbMove();
    }

    if (event->buttons() == Qt::NoButton) {
        m_situation->stateMachine()->move();
    }

    // trigger next update rate calculation
    m_time.start();
}

/**
    Performs action depending of button and mode
    This method will trigger performing of actions like:
    - setting position of selected objects
    - setting heading of selected BoatGraphicsItem
    - creating new TrackModel, BoatModel or MarkModel according to mode.
*/

void SituationScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mouseReleaseEvent(event);

    if (m_clickState != SINGLE) {
        m_clickState = SINGLE;
        return;
    }
    mouseClickEvent(event);
}

void SituationScene::mouseClickEvent(QGraphicsSceneMouseEvent *event) {
    if (debugLevel & 1 << VIEW) std::cout << "Mouse Release " << m_situation->selectedModels().size() << std::endl;

    // do not deliver events in animation mode
    if (m_situation->stateMachine()->animationState()->active()) {
        return;
    }

    bool click = (m_clickTime.elapsed() < 250);
    if (click && (event->button() == Qt::RightButton
                || (event->button() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0)))) {
        if (!m_situation->selectedModels().isEmpty()) {
            switch(selectedItems()[0]->type()) {
                case BOAT_TYPE: {
                    m_boatPopup->popup(event->screenPos());
                    return;
                }
                break;

                case MARK_TYPE: {
                    m_markPopup->popup(event->screenPos());
                    return;
                }
                break;
                case POINT_TYPE: {
                    m_pointPopup->popup(event->screenPos());
                    return;
                }
                break;
            }
        }
        m_defaultPopup->popup(event->screenPos());
        return;
    }

    if (event->button() == Qt::RightButton
        || (event->button() == Qt::LeftButton
           && ((event->modifiers() & Qt::MetaModifier) != 0))) {
        m_situation->stateMachine()->rmbclick();
    }
    if (event->button() == Qt::LeftButton) {
        m_situation->stateMachine()->lmbclick();
    }
    if (m_situation->selectedModels().empty()) {
        m_situation->stateMachine()->clearSelection();
    }
}

void SituationScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
    m_clickState = DOUBLE;
    m_situation->stateMachine()->noState();
    m_situation->undo();
}

void SituationScene::setSelectedModels() {
    if (debugLevel & 1 << VIEW) std::cout << "SelectedModels update " << m_situation->selectedModels().size() << std::endl;
    emit selectedModelsChanged();
}

void SituationScene::setLaylines(const int angle) {
    if (!m_situation->showLayline()) {
        if (debugLevel & 1 << VIEW) std::cout << "reseting empty Background" << std::endl;
        setBackgroundBrush(Qt::NoBrush);
        return;
    }
    if (debugLevel & 1 << VIEW) std::cout << "creating layline Background for " << angle << std::endl;
    qreal theta = angle * M_PI /180;
    int length = Boats::seriesSizeList()[m_situation->situationSeries()];

    // draw 4 times as big, then use transform to scale back the brush
    // gives better precision grid
    qreal x = 2*length*sin(theta) * 4;
    qreal y = 2*length*cos(theta) * 4;

    QPixmap pixmap(x,y);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPen pen;
    pen.setWidth(2);
    QVector<qreal> dashes;
    dashes << length/10.0 << length/5.0;
    pen.setStyle(Qt::CustomDashLine);
    pen.setDashPattern(dashes);
    painter.setPen(pen);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.drawLine(QLineF(0,0,x,y));
    painter.drawLine(QLineF(0,y,x,0));

    QBrush brush;
    QTransform transform;
    transform.scale(0.25, 0.25);
    brush.setTransform(transform);
    brush.setTexture(pixmap);
    setBackgroundBrush(brush);
}
