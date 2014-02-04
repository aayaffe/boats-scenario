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
#include "undocommands.h"

#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"
#include "windmodel.h"

#include "boat.h"
#include "track.h"
#include "mark.h"
#include "polyline.h"
#include "point.h"
#include "arrow.h"
#include "trackanimation.h"
#include "scenarioanimation.h"
#include "angleanimation.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

#include <iostream>
#include <cmath>

extern int debugLevel;

SituationScene::SituationScene(SituationModel *situation)
        : QGraphicsScene(situation),
        m_situation(situation),
        m_scenarioAnimation(new ScenarioAnimation),
        m_trackCreated(0),
        m_state(SituationModel::NO_STATE),
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

    connect(situation, SIGNAL(stateChanged(SituationModel::SceneState)),
            this, SLOT(setState(SituationModel::SceneState)));

    setLaylines(situation->laylineAngle());

}

void SituationScene::setState(const SituationModel::SceneState& theValue, bool commit) {
    // undo previous state's settings
    switch(m_state) {
    case SituationModel::CREATE_TRACK:
    case SituationModel::CREATE_BOAT:
    case SituationModel::CREATE_MARK:
    case SituationModel::CREATE_LINE:
    case SituationModel::CREATE_POINT: {
            m_situation->undoStack()->endMacro();
            if (!commit) {
                m_situation->undoStack()->undo();
            }
        }
        break;
    default:
        break;
    }
    switch(theValue) {
    case SituationModel::CREATE_TRACK: {
            m_trackCreated = m_situation->createTrack(m_curPosition);
        }
        break;
    case SituationModel::CREATE_BOAT: {
            m_situation->createBoat(m_trackCreated, m_curPosition);
        }
        break;
    case SituationModel::CREATE_MARK: {
            m_markCreated = m_situation->createMark(m_curPosition);
        }
        break;
    case SituationModel::CREATE_LINE: {
            m_polyLineCreated = m_situation->createLine(m_curPosition);
        }
        break;
    case SituationModel::CREATE_POINT: {
            m_situation->createPoint(m_polyLineCreated, m_curPosition);
        }
        break;
    default:
        break;
    }
    m_state = theValue;
    emit stateChanged(m_state);
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
    Prepares the Scene for animation mode.
    This method finds the maximum size of track, and sets the timer length
    accordingly.
    It then creates a BoatGraphicsItem for animation purpose, and associates
    an TrackAnimation.
*/

void SituationScene::setAnimation() {
    if (debugLevel & 1 << VIEW) std::cout << "preparing for Animation" << std::endl;
    int maxSize = 0;
    foreach (const TrackModel *track, m_situation->tracks()) {
        if (track->boats().size() > maxSize)
            maxSize = track->boats().size() - 1;
    }

    m_windAnimation = new QSequentialAnimationGroup(this);
    for (int i = 0; i < m_situation->wind().size()-1; ++i) {
        AngleAnimation *wind = new AngleAnimation(&m_situation->wind(), "direction");
        wind->setDuration(2000);
        wind->setStartValue(m_situation->wind().windAt(i));
        wind->setEndValue(m_situation->wind().windAt(i+1));
        m_windAnimation->addAnimation(wind);
    }
    m_scenarioAnimation->addAnimation(m_windAnimation);


    foreach (TrackModel *track, m_situation->tracks()) {
        BoatModel *boat = track->boats()[0];
        BoatGraphicsItem *boatItem = new BoatGraphicsItem(new BoatModel(track));
        addItem(boatItem);
        boatItem->boat()->setOrder(0);
        boatItem->setPosition(boat->position());
        boatItem->setHeading(boat->heading());
        boatItem->boat()->setWind(m_situation->wind().windAt(0));
        boatItem->boat()->setLaylines(boat->laylines());
        connect(&track->situation()->wind(), SIGNAL(directionChanged(qreal)),
                boatItem->boat(), SLOT(setWind(qreal)));
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
        shadow->setXOffset(4);
        shadow->setYOffset(4);
        shadow->setBlurRadius(4);
        boatItem->setGraphicsEffect(shadow);

        TrackAnimation *animation = new TrackAnimation(track, boatItem->boat(), m_scenarioAnimation);
        m_scenarioAnimation->addAnimation(animation);
        m_animationItems.push_back(animation);

        if(track->followTrack()) {
            connect(boatItem->boat(), SIGNAL(positionChanged(QPointF)),
                    this, SIGNAL(centerChanged(QPointF)));
            disconnect(m_situation, SIGNAL(lookDirectionChanged(qreal)));
            connect(boatItem->boat(), SIGNAL(headingChanged(qreal)),
                    this, SIGNAL(lookDirectionChanged(qreal)));
        }
    }

    foreach (MarkModel *mark, m_situation->marks()) {
        connect(&m_situation->wind(), SIGNAL(directionChanged(qreal)),
                mark, SLOT(setWind(qreal)));
    }

    foreach(PolyLineModel *poly, m_situation->polyLines()) {
        foreach(PointModel *point, poly->points()) {
            connect(&m_situation->wind(), SIGNAL(directionChanged(qreal)),
                    point, SLOT(setWind(qreal)));
        }
    }
}

/**
    Restores the Scene out of animation mode.
    This method brings the scene back to the normal drawing mode.
    For this it removes all TrackAnimation objects created in setAnimation().
*/

void SituationScene::unSetAnimation() {
    if (debugLevel & 1 << VIEW) std::cout << "ending Animation" << std::endl;

    m_situation->wind().setDirection(m_situation->wind().windAt(0));
    m_scenarioAnimation->removeAnimation(m_windAnimation);
    for (int i = 0; i < m_situation->wind().size()-1; ++i) {
        QAbstractAnimation *animation = m_windAnimation->animationAt(0);
        m_windAnimation->removeAnimation(animation);
        delete animation;
    }
    delete m_windAnimation;

    foreach (TrackAnimation *animation, m_animationItems) {
        // the boat was never really part of the track, we use situation signal
        // directly to have the graphicsitem removed
        animation->boat()->track()->situation()->removingBoat(animation->boat());
        m_scenarioAnimation->removeAnimation(animation);
        m_animationItems.removeOne(animation);
        delete animation->boat();
        delete animation;
    }

    foreach (MarkModel *mark, m_situation->marks()) {
        disconnect(mark, SLOT(setWind(qreal)));
    }
}

/**
    Reacts to user keyboard actions in the Scene
    This method modifies the associated SituationModel through the
    concerned Undo Framework class. Handled keys are for
    - heading of selected boats (+,-)
    - position of selected objects (H (left), L (right), J (down), K (up))
*/

void SituationScene::keyPressEvent(QKeyEvent *event) {

    if (!m_situation->selectedBoatModels().isEmpty()) {
        QList<BoatModel*> selectedBoatModels = m_situation->selectedBoatModels();
// Trim just the jib when ',' or '.' is pressed (TrimJibUndoCommand only trims the jib)
// But not when Ctrl is pressed - reserve that for spinTrim
        if (event->key() == Qt::Key_Comma && !(event->modifiers() & Qt::ControlModifier)) {
            m_situation->undoStack()->push(new TrimJibUndoCommand(selectedBoatModels, selectedBoatModels[0]->jibTrim() - 5));
        } else if (event->key() == Qt::Key_Period && !(event->modifiers() & Qt::ControlModifier)) {
            m_situation->undoStack()->push(new TrimJibUndoCommand(selectedBoatModels, selectedBoatModels[0]->jibTrim() + 5));
// Trim the spin when Ctrl+',' or Ctrl+'.' is pressed
        }else if (event->key() == Qt::Key_Comma && (event->modifiers() & Qt::ControlModifier)) {
            m_situation->undoStack()->push(new TrimSpinUndoCommand(selectedBoatModels, selectedBoatModels[0]->spinTrim() - 5));
        } else if (event->key() == Qt::Key_Period && (event->modifiers() & Qt::ControlModifier)) {
            m_situation->undoStack()->push(new TrimSpinUndoCommand(selectedBoatModels, selectedBoatModels[0]->spinTrim() + 5));
        }
    }

    if (!m_situation->selectedModels().isEmpty()) {
        QList<PositionModel*> selectedModels = m_situation->selectedModels();
        if (event->key() == Qt::Key_Left) {
            QPointF pos(-5,0);
            m_situation->moveModel(pos);

        } else if (event->key() == Qt::Key_Right) {
            QPointF pos(5,0);
            m_situation->moveModel(pos);

        } else if (event->key() == Qt::Key_Up) {
            QPointF pos(0,-5);
            m_situation->moveModel(pos);

        } else if (event->key() == Qt::Key_Down) {
            QPointF pos(0,5);
            m_situation->moveModel(pos);

        } else if (event->key() == Qt::Key_Plus) {
            m_situation->undoStack()->push(new RotateModelsUndoCommand(selectedModels, 5.0));

        } else if (event->key() == Qt::Key_Minus) {
            m_situation->undoStack()->push(new RotateModelsUndoCommand(selectedModels, -5.0));

        } else {
            QGraphicsScene::keyPressEvent(event);
        }
    } else {
        QGraphicsScene::keyPressEvent(event);
    }
}

/**
    Sets selection depending on modal status.
    Selection happens with left button in NO_STATE, and right button in CREATE_BOAT
    and CREATE_MARK modes.
*/

void SituationScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    switch (m_state) {
        case SituationModel::NO_STATE:
            mouseSelectEvent(event);
            if (!m_situation->selectedBoatModels().isEmpty()) {
                m_trackCreated = m_situation->selectedBoatModels()[0]->track();
            }
            if (!m_situation->selectedPointModels().isEmpty()) {
                m_polyLineCreated = m_situation->selectedPointModels()[0]->polyLine();
            }
            break;
        case SituationModel::CREATE_TRACK:
        case SituationModel::CREATE_BOAT:
        case SituationModel::CREATE_MARK:
            if (event->buttons() == Qt::RightButton
                || (event->buttons() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                mouseSelectEvent(event);
            }
            break;
        default:
            break;
    }

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

    switch (m_state) {
        case SituationModel::NO_STATE:
            if (event->buttons() == Qt::LeftButton
                && (event->modifiers() & Qt::MetaModifier) == 0) {
                m_situation->moveModel(event->scenePos() - m_fromPosition);
                m_fromPosition = event->scenePos();
            }
            if (event->buttons() == Qt::RightButton
                || (event->buttons() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                m_situation->headingModel(event->scenePos() - m_modelPressed->position());
            }
        break;
        case SituationModel::CREATE_TRACK:
            if (event->buttons() == Qt::NoButton) {
                m_trackCreated->boats().last()->setPosition(event->scenePos());
            }
            if (event->buttons() == Qt::RightButton
                || (event->buttons() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                m_situation->headingModel(event->scenePos() - m_modelPressed->position());
            }
            break;
        case SituationModel::CREATE_BOAT:
            if (event->buttons() == Qt::NoButton) {
                qreal heading = m_trackCreated->headingForNext(
                        m_trackCreated->boats().size()-2, event->scenePos());
                m_trackCreated->boats().last()->setHeading(heading);
                m_trackCreated->boats().last()->setPosition(event->scenePos());
            }
            if (event->buttons() == Qt::RightButton
                || (event->buttons() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                m_situation->headingModel(event->scenePos() - m_modelPressed->position());
            }
            break;
        case SituationModel::CREATE_MARK:
            if (event->buttons() == Qt::NoButton) {
                m_markCreated->setPosition(event->scenePos());
            }
            break;
        case SituationModel::CREATE_LINE:
        case SituationModel::CREATE_POINT:
            if (event->buttons() == Qt::NoButton) {
                m_polyLineCreated->points().last()->setPosition(event->scenePos());
            }
            break;
        default:
            break;
    }

    // trigger next update rate calculation
    m_time.start();
    m_curPosition = event->scenePos();
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

    switch (m_state) {
        case SituationModel::NO_STATE:
            if (event->button() == Qt::LeftButton
                && (event->modifiers() & Qt::MetaModifier) == 0) {
                m_situation->moveModel(event->scenePos() - m_fromPosition);
                m_fromPosition = event->scenePos();
            }
            if (event->button() == Qt::RightButton
                || (event->button() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                m_situation->headingModel(event->scenePos() - m_modelPressed->position());
            }
            break;
        case SituationModel::CREATE_TRACK:
            if (event->button() == Qt::LeftButton) {
                setState(SituationModel::CREATE_BOAT, true);
            }
            break;
        case SituationModel::CREATE_BOAT:
            if (event->button() == Qt::LeftButton) {
                if ((event->modifiers() & Qt::ControlModifier) != 0) {
                    setState(SituationModel::CREATE_BOAT, true);
                } else if ((event->modifiers() & Qt::MetaModifier) == 0) {
                    m_situation->createBoat(m_trackCreated, event->scenePos());
                }
            }
            if (event->button() == Qt::RightButton
                || (event->button() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                m_situation->headingModel(event->scenePos() - m_modelPressed->position());
            }
            break;
        case SituationModel::CREATE_MARK:
            if (event->button() == Qt::LeftButton) {
                m_markCreated = m_situation->createMark(event->scenePos());
            }
            break;
        case SituationModel::CREATE_LINE:
            if (event->button() == Qt::LeftButton) {
                setState(SituationModel::CREATE_POINT, true);
            }
            break;
        case SituationModel::CREATE_POINT:
            if (event->button() == Qt::LeftButton) {
                m_situation->createPoint(m_polyLineCreated, event->scenePos());
            }
        default:
            break;
    }
}

void SituationScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
    m_clickState = DOUBLE;
    m_situation->setState(SituationModel::NO_STATE);
    m_situation->undoStack()->undo();
}

void SituationScene::mouseSelectEvent(QGraphicsSceneMouseEvent *event) {
    // propagate mouse event first for selected items
    QGraphicsScene::mousePressEvent(event);

    m_fromPosition = event->scenePos();
    if (debugLevel & 1 << VIEW) std::cout << "Mouse pressed with " << m_situation->selectedModels().size()
    << " items selected" << std::endl;
}

void SituationScene::setSelectedModels() {
    m_situation->clearSelectedModels();
    foreach (QGraphicsItem *item, selectedItems()) {
        switch(item->type()) {
            case BOAT_TYPE: {
                BoatModel *boat = (qgraphicsitem_cast<BoatGraphicsItem*>(item))->boat();
                m_situation->addSelectedBoat(boat);
                }
                break;
            case MARK_TYPE: {
                MarkModel *mark = (qgraphicsitem_cast<MarkGraphicsItem*>(item))->mark();
                m_situation->addSelectedMark(mark);
                }
                break;
            case POINT_TYPE: {
                PointModel *point = (qgraphicsitem_cast<PointGraphicsItem*>(item))->point();
                m_situation->addSelectedPoint(point);
                }
                break;
            case ARROW_TYPE: {
                WindModel *wind = &m_situation->wind();
                m_situation->addSelectedModel(wind);
            }
        }
    }
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
