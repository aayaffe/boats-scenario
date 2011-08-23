//
// C++ Implementation: situationscene
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
#include <cmath>

#include <QtGui>

#include "situationscene.h"

#include "commontypes.h"
#include "undocommands.h"

#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"

#include "boat.h"
#include "track.h"
#include "mark.h"
#include "polyline.h"
#include "point.h"
#include "trackanimation.h"
#include "scenarioanimation.h"

extern int debugLevel;

SituationScene::SituationScene(SituationModel *situation)
        : QGraphicsScene(situation),
        m_situation(situation),
        m_scenarioAnimation(new ScenarioAnimation),
        m_trackCreated(0),
        m_state(NO_STATE),
        m_time(QTime::currentTime()),
        m_clickTime(QTime::currentTime()),
        m_clickState(SINGLE),
        m_actionMenu(0) {

    // try to set a minimum scene rect
    QGraphicsItem *e1 = addEllipse(QRectF(-1000,-1000, 1, 1));
    QGraphicsItem *e2 = addEllipse(QRectF(1000, 1000, 1, 1));
    sceneRect();
    delete e1;
    delete e2;

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

    setLaylines(situation->laylineAngle());
}

void SituationScene::setState(const SceneState& theValue, bool commit) {
    // undo previous state's settings
    switch(m_state) {
    case CREATE_TRACK:
    case CREATE_BOAT:
    case CREATE_MARK:
    case CREATE_LINE:
    case CREATE_POINT: {
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
    case CREATE_TRACK: {
            createTrack(m_curPosition);
        }
        break;
    case CREATE_BOAT: {
            createBoat(m_curPosition);
        }
        break;
    case CREATE_MARK: {
            createMark(m_curPosition);
        }
        break;
    case CREATE_LINE: {
            createLine(m_curPosition);
        }
        break;
    case CREATE_POINT: {
            createPoint(m_curPosition);
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

    foreach (TrackModel *track, m_situation->tracks()) {
        BoatGraphicsItem *boatItem = new BoatGraphicsItem(new BoatModel(track));
        addItem(boatItem);
        boatItem->setOrder(0);
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
        shadow->setXOffset(4);
        shadow->setYOffset(4);
        shadow->setBlurRadius(4);
        boatItem->setGraphicsEffect(shadow);

        TrackAnimation *animation = new TrackAnimation(track, boatItem->boat(), m_scenarioAnimation);
        m_scenarioAnimation->addAnimation(animation);
        m_animationItems.push_back(animation);
    }
}

/**
    Restores the Scene out of animation mode.
    This method brings the scene back to the normal drawing mode.
    For this it removes all TrackAnimation objects created in setAnimation().
*/

void SituationScene::unSetAnimation() {
    if (debugLevel & 1 << VIEW) std::cout << "ending Animation" << std::endl;
    foreach (TrackAnimation *animation, m_animationItems) {
        // the boat was never really part of the track, we use situation signal
        // directly to have the graphicsitem removed
        animation->boat()->track()->situation()->removingBoat(animation->boat());
        m_scenarioAnimation->removeAnimation(animation);
        m_animationItems.removeOne(animation);
        delete animation->boat();
        delete animation;
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

    if (!m_selectedBoatModels.isEmpty()) {
        if (event->key() == Qt::Key_Plus) {
            qreal theta = fmod(m_selectedBoatModels[0]->heading() + 5 + 360.0, 360.0);
            m_situation->undoStack()->push(new HeadingBoatUndoCommand(m_selectedBoatModels, theta));

        } else if (event->key() == Qt::Key_Minus) {
            qreal theta = fmod(m_selectedBoatModels[0]->heading() - 5 + 360.0, 360.0);
            m_situation->undoStack()->push(new HeadingBoatUndoCommand(m_selectedBoatModels, theta));
        }
    }

    if (!m_selectedModels.isEmpty()) {
        if (event->key() == Qt::Key_Left) {
            QPointF pos(-5,0);
            m_situation->undoStack()->push(new MoveModelUndoCommand(m_selectedModels, pos));

        } else if (event->key() == Qt::Key_Right) {
            QPointF pos(5,0);
            m_situation->undoStack()->push(new MoveModelUndoCommand(m_selectedModels, pos));

        } else if (event->key() == Qt::Key_Up) {
            QPointF pos(0,-5);
            m_situation->undoStack()->push(new MoveModelUndoCommand(m_selectedModels, pos));

        } else if (event->key() == Qt::Key_Down) {
            QPointF pos(0,5);
            m_situation->undoStack()->push(new MoveModelUndoCommand(m_selectedModels, pos));
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
        case NO_STATE:
            mouseSelectEvent(event);
            if (!m_selectedBoatModels.isEmpty()) {
                m_trackCreated = m_selectedBoatModels[0]->track();
            }
            if (!m_selectedPointModels.isEmpty()) {
                m_polyLineCreated = m_selectedPointModels[0]->polyLine();
            }
            break;
        case CREATE_TRACK:
        case CREATE_BOAT:
        case CREATE_MARK:
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
        case NO_STATE:
            if (event->buttons() == Qt::LeftButton
                && (event->modifiers() & Qt::MetaModifier) == 0) {
                moveModel(event->scenePos());
            }
            if (event->buttons() == Qt::RightButton
                || (event->buttons() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                headingBoat(event->scenePos());
            }
        break;
        case CREATE_TRACK:
            if (event->buttons() == Qt::NoButton) {
                m_trackCreated->boats().last()->setPosition(event->scenePos());
            }
            if (event->buttons() == Qt::RightButton
                || (event->buttons() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                headingBoat(event->scenePos());
            }
            break;
        case CREATE_BOAT:
            if (event->buttons() == Qt::NoButton) {
                qreal heading = m_trackCreated->headingForNext(
                        m_trackCreated->boats().size()-2, event->scenePos());
                m_trackCreated->boats().last()->setHeading(heading);
                m_trackCreated->boats().last()->setPosition(event->scenePos());
            }
            if (event->buttons() == Qt::RightButton
                || (event->buttons() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                headingBoat(event->scenePos());
            }
            break;
        case CREATE_MARK:
            if (event->buttons() == Qt::NoButton) {
                m_markCreated->setPosition(event->scenePos());
            }
            break;
        case CREATE_LINE:
        case CREATE_POINT:
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
        m_actionMenu->popup(event->screenPos());
        return;
    }

    switch (m_state) {
        case NO_STATE:
            if (event->button() == Qt::LeftButton
                && (event->modifiers() & Qt::MetaModifier) == 0) {
                moveModel(event->scenePos());
            }
            if (event->button() == Qt::RightButton
                || (event->button() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                headingBoat(event->scenePos());
            }
            break;
        case CREATE_TRACK:
            if (event->button() == Qt::LeftButton) {
                setState(CREATE_BOAT, true);
            }
            break;
        case CREATE_BOAT:
            if (event->button() == Qt::LeftButton) {
                if ((event->modifiers() & Qt::ControlModifier) != 0) {
                    setState(CREATE_BOAT, true);
                } else if ((event->modifiers() & Qt::MetaModifier) == 0) {
                    createBoat(event->scenePos());
                }
            }
            if (event->button() == Qt::RightButton
                || (event->button() == Qt::LeftButton
                    && ((event->modifiers() & Qt::MetaModifier) != 0))) {
                headingBoat(event->scenePos());
            }
            break;
        case CREATE_MARK:
            if (event->button() == Qt::LeftButton) {
                createMark(event->scenePos());
            }
            break;
        case CREATE_LINE:
            if (event->button() == Qt::LeftButton) {
                setState(CREATE_POINT, true);
            }
            break;
        case CREATE_POINT:
            if (event->button() == Qt::LeftButton) {
                createPoint(event->scenePos());
            }
        default:
            break;
    }
}

void SituationScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
    m_clickState = DOUBLE;
    setState(NO_STATE);
    m_situation->undoStack()->undo();
}

void SituationScene::mouseSelectEvent(QGraphicsSceneMouseEvent *event) {
    // propagate mouse event first for selected items
    QGraphicsScene::mousePressEvent(event);

    m_fromPosition = event->scenePos();
    if (debugLevel & 1 << VIEW) std::cout << "Mouse pressed with " << m_selectedModels.size()
    << " items selected" << std::endl;
}

void SituationScene::moveModel(QPointF pos) {
    if (!m_selectedModels.isEmpty() && pos != m_fromPosition) {
        m_situation->undoStack()->push(new MoveModelUndoCommand(m_selectedModels,(pos-m_fromPosition)));
        m_fromPosition = pos;
    }
}

void SituationScene::headingBoat(QPointF pos) {
    if (!m_selectedBoatModels.isEmpty() && pos != m_modelPressed->position()) {
        QPointF point = pos - m_modelPressed->position();
        qreal theta = fmod((atan2 (point.x(), -point.y()) * 180 / M_PI) + 360.0, 360.0);
        qreal snap = m_situation->laylineAngle();
        if (fabs(theta)<=5) {
            theta = 0;
        } else if (fabs(theta-snap)<=5) {
            theta = snap;
        } else if (fabs(theta-(180-snap)) <=5) {
            theta = 180-snap;
        } else if (fabs(theta-180)<=5) {
            theta = 180;
        } else if (fabs(theta-(180+snap)) <=5) {
            theta = 180+snap;
        } else if (fabs(theta-(360-snap)) <=5) {
            theta = 360-snap;
        }
        m_situation->undoStack()->push(new HeadingBoatUndoCommand(m_selectedBoatModels, theta));
    }
}

void SituationScene::createTrack(QPointF pos) {
    AddTrackUndoCommand *command = new AddTrackUndoCommand(m_situation);
    m_situation->undoStack()->push(command);
    TrackModel *track = command->track();
    BoatModel *boat = new BoatModel(track, track);
    boat->setDim(64);
    boat->setPosition(pos);
    track->addBoat(boat);
    m_trackCreated = track;
}

void SituationScene::createBoat(QPointF pos) {
    if (m_trackCreated) {
        m_situation->undoStack()->endMacro();
        m_trackCreated->boats().last()->setDim(255);
        qreal heading = m_trackCreated->boats().last()->heading();
        AddBoatUndoCommand *command = new AddBoatUndoCommand(m_trackCreated, pos, heading);
        command->boat()->setDim(64);
        m_situation->undoStack()->beginMacro("");
        m_situation->undoStack()->push(command);
    }
}

void SituationScene::createMark(QPointF pos) {
    m_situation->undoStack()->endMacro();
    AddMarkUndoCommand *command = new AddMarkUndoCommand(m_situation, pos);
    m_markCreated = command->mark();
    m_situation->undoStack()->beginMacro("");
    m_situation->undoStack()->push(command);
}

void SituationScene::createLine(QPointF pos) {
    AddPolyLineUndoCommand *command = new AddPolyLineUndoCommand(m_situation);
    m_situation->undoStack()->push(command);
    m_polyLineCreated = command->polyLine();
    PointModel *point = new PointModel(m_polyLineCreated);
    point->setPosition(pos);
    m_polyLineCreated->addPoint(point);
}

void SituationScene::createPoint(QPointF pos) {
    if (m_polyLineCreated) {
        m_situation->undoStack()->endMacro();
        AddPointUndoCommand *command = new AddPointUndoCommand(
                m_polyLineCreated, pos);
        m_situation->undoStack()->beginMacro("");
        m_situation->undoStack()->push(command);
    }
}

void SituationScene::setSelectedModels() {
    m_selectedModels.clear();
    m_selectedBoatModels.clear();
    m_selectedMarkModels.clear();
    m_selectedPointModels.clear();
    foreach (QGraphicsItem *item, selectedItems()) {
        switch(item->type()) {
            case BOAT_TYPE: {
                BoatModel *boat = (qgraphicsitem_cast<BoatGraphicsItem*>(item))->boat();
                m_selectedModels << boat;
                m_selectedBoatModels << boat;
                }
                break;
            case MARK_TYPE: {
                MarkModel *mark = (qgraphicsitem_cast<MarkGraphicsItem*>(item))->mark();
                m_selectedModels << mark;
                m_selectedMarkModels << mark;
                }
                break;
            case POINT_TYPE: {
                PointModel *point = (qgraphicsitem_cast<PointGraphicsItem*>(item))->point();
                m_selectedModels << point;
                m_selectedPointModels << point;
                }
                break;
        }
    }
    if (debugLevel & 1 << VIEW) std::cout << "SelectedModels update " << m_selectedModels.size() << std::endl;
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
    int length = m_situation->sizeForSeries(m_situation->situationSeries());

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
