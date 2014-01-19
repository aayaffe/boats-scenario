//
// C++ Implementation: situationmodel
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

#include <iostream>

#include "situationmodel.h"

#include "commontypes.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"
#include "undocommands.h"

extern int debugLevel;

SituationModel::SituationModel(QObject *parent)
        : QObject(parent),
        m_showLayline(true),
        m_laylineAngle(40),
        m_situationSeries(Boats::keelboat),
        m_situationLength(3),
        m_lookDirection(0),
        m_tilt(0),
        m_wind(this),
        m_undoStack(new QUndoStack(this)) {
    if (debugLevel & 1 << MODEL) std::cout << "new situation " << this << std::endl;
    connect(&m_wind, SIGNAL(windReset()),
            this, SLOT(resetWind()));
}

SituationModel::~SituationModel() {
    if (debugLevel & 1 << MODEL) std::cout << "end situation " << this << std::endl;
}

void SituationModel::setTitle(const QString theValue) {
    if (theValue != m_title) {
        if (debugLevel & 1 << MODEL) std::cout << "Setting Title " << theValue.toStdString() << std::endl;
        m_title = theValue;
        emit titleChanged(m_title);
    }
}

void SituationModel::setRules(const QString theValue) {
    if (theValue != m_rules) {
        if (debugLevel & 1 << MODEL) std::cout << "Setting Rules " << theValue.toStdString() << std::endl;
       m_rules = theValue;
       emit rulesChanged(m_rules);
    }
}

void SituationModel::setShowLayline(const bool theValue) {
    if (theValue != m_showLayline) {
        if (debugLevel & 1 << MODEL) std::cout << "Show Layline " << theValue << std::endl;
        m_showLayline = theValue;
        emit showLaylineChanged(m_showLayline);
        emit laylineChanged(m_laylineAngle);
    }
}

void SituationModel::setLaylineAngle(const int theValue) {
    if (theValue != m_laylineAngle) {
        if (debugLevel & 1 << MODEL) std::cout << "Situation " << this
        << " Layline Angle " << theValue << std::endl;
        m_laylineAngle = theValue;
        emit laylineChanged(m_laylineAngle);
    }
}

void SituationModel::setSituationSeries(const int theValue) {
    if (theValue != m_situationSeries) {
        if (debugLevel & 1 << MODEL) std::cout << "Situation " << this
        << " Series " << ENUM_NAME(Boats, Series, theValue) << std::endl;
        m_situationSeries = (Boats::Series) theValue;
        emit seriesChanged(m_situationSeries);
        emit laylineChanged(m_laylineAngle);
    }
}

void SituationModel::setSituationLength(const int theValue) {
    if (theValue != m_situationLength) {
        if (debugLevel & 1 << MODEL) std::cout << "Situation " << this
        << " Length " << theValue << std::endl;
        m_situationLength = theValue;
        foreach (MarkModel *mark, m_marks) {
            mark->setLength(m_situationLength);
        }
        emit lengthChanged(m_situationLength);
    }
}

void SituationModel::setAbstract(const QString theValue) {
    if (theValue != m_abstract) {
        if (debugLevel & 1 << MODEL) std::cout << "Setting Abstract " << theValue.toStdString() << std::endl;
        m_abstract = theValue;
        emit abstractChanged(m_abstract);
    }
}

void SituationModel::setDescription(const QString theValue) {
    if (theValue != m_description) {
        if (debugLevel & 1 << MODEL) std::cout << "Setting Description " << theValue.toStdString() << std::endl;
        m_description = theValue;
        emit descriptionChanged(m_description);
    }
}

void SituationModel::setLookDirection(qreal theValue) {
    if (theValue != m_lookDirection) {
        if (debugLevel & 1 << MODEL) std::cout << "Setting lookDirection " << theValue << std::endl;
        m_lookDirection = theValue;
        emit lookDirectionChanged(m_lookDirection);
    }
}

void SituationModel::setTilt(qreal theValue) {
    if (theValue != m_tilt) {
        if (debugLevel & 1 << MODEL) std::cout << "Setting tilt " << theValue << std::endl;
        m_tilt = theValue;
        emit tiltChanged(m_tilt);
    }
}

void SituationModel::appendDiscardedXml(const QString& theValue) {
    if (!m_discardedXml.contains(theValue)) {
        m_discardedXml.append(theValue);
    }
}

void SituationModel::addTrack(TrackModel *track, int order) {
    if (order == -1) {
        order = m_tracks.size();
    }
    m_tracks.insert(order,track);
    if (debugLevel & 1 << MODEL) std::cout << "Adding Track " << order+1 <<  std::endl;
    for (int i=order+1; i<m_tracks.size(); i++) {
        m_tracks[i]->setOrder(i+1);
    }
    track->displayBoats();
    emit trackAdded(track);
}

void SituationModel::deleteTrack(TrackModel *track) {
    int index = m_tracks.indexOf(track);
    if (debugLevel & 1 << MODEL) std::cout << "Removing Track " << index+1
    << " with " << track->size() << std::endl;
    track->hideBoats();
    m_tracks.removeOne(track);
    for (int i=index; i<m_tracks.size(); i++) {
        m_tracks[i]->setOrder(i+1);
    }
    emit trackRemoved(track);
}

void SituationModel::addMark(MarkModel *mark, int order) {
    if (order == -1) {
        order = m_marks.size();
    }
    m_marks.insert(order, mark);
    if (debugLevel & 1 << MODEL) std::cout << "Adding Mark " << order+1 << std::endl;
    for (int i=order+1; i<m_marks.size(); i++) {
        m_marks[i]->setOrder(i+1);
    }
    emit markAdded(mark);
}

int SituationModel::deleteMark(MarkModel *mark) {
    int index = m_marks.indexOf(mark);
    if (debugLevel & 1 << MODEL) std::cout << "Removing Mark " << index+1 << std::endl;
    m_marks.removeOne(mark);
    for (int i=index; i<m_marks.size(); i++) {
        m_marks[i]->setOrder(i+1);
    }
    emit markRemoved(mark);
    return index;
}

void SituationModel::addPolyLine(PolyLineModel *polyline, int order) {
    if (order == -1) {
        order = m_lines.size();
    }
    m_lines.insert(order,polyline);
    if (debugLevel & 1 << MODEL) std::cout << "Adding PolyLine " << order+1 <<  std::endl;
    polyline->displayPoints();
    emit polyLineAdded(polyline);
}

void SituationModel::deletePolyLine(PolyLineModel *polyline) {
    int index = m_lines.indexOf(polyline);
    if (debugLevel & 1 << MODEL) std::cout << "Removing Line " << index+1
    << " with " << polyline->size() << std::endl;
    polyline->hidePoints();
    m_lines.removeOne(polyline);
    emit polyLineRemoved(polyline);
}

void SituationModel::resetWind() {
    if (debugLevel & 1 << MODEL) std::cout << "Resetting Wind " << std::endl;
    foreach (TrackModel *track, m_tracks) {
        foreach (BoatModel *boat, track->boats()) {
            boat->setWind(m_wind.windAt(boat->order()-1));
        }
    }
    foreach (MarkModel *mark, m_marks) {
        mark->setWind(m_wind.windAt(0));
    }
}

void SituationModel::setState(const SceneState& theValue, bool commit) {
    m_state = theValue;
    emit stateChanged(m_state);
}

void SituationModel::moveModel(QPointF pos) {
    if (!m_selectedModels.isEmpty() && pos != QPointF()) {
        m_undoStack->push(new MoveModelUndoCommand(m_selectedModels,pos));
    }
}

void SituationModel::headingModel(QPointF pos) {
    if (!m_selectedModels.isEmpty() && pos != QPointF()) {
        qreal wind = m_selectedModels.last()->wind();
        qreal theta = fmod((atan2 (pos.x(), -pos.y()) * 180 / M_PI) + 360.0, 360.0);
        qreal delta = fmod(theta - wind + 360, 360);
        qreal snap = m_laylineAngle;
        // face to wind
        if (fabs(delta)<=5) {
            theta = wind;
        // port closehauled
        } else if (fabs(delta - snap)<=5) {
            theta = fmod(wind + snap, 360);
        // port downwind
        } else if (fabs(delta -(180-snap)) <=5) {
            theta = fmod(wind + 180-snap, 360);
        // deadwind
        } else if (fabs(delta - 180)<=5) {
            theta = fmod(wind - 180, 360);
        // starboard downwind
        } else if (fabs(delta - (180+snap)) <=5) {
            theta = fmod(wind + 180 + snap, 360);
        // starboard closehauled
        } else if (fabs(delta - (360-snap)) <=5) {
            theta = fmod(wind + 360 - snap, 360);
        }
        m_undoStack->push(new RotateModelsUndoCommand(m_selectedModels, theta-m_selectedModels.last()->heading()));
    }
}

TrackModel *SituationModel::createTrack(QPointF pos) {
    AddTrackUndoCommand *command = new AddTrackUndoCommand(this);
    m_undoStack->push(command);
    TrackModel *track = command->track();
    BoatModel *boat = new BoatModel(track, track);
    boat->setDim(64);
    boat->setPosition(pos);
    track->addBoat(boat);
    return track;
}

BoatModel *SituationModel::createBoat(TrackModel *track, QPointF pos) {
    m_undoStack->endMacro();
    track->boats().last()->setDim(255);
    qreal heading = track->boats().last()->heading();
    AddBoatUndoCommand *command = new AddBoatUndoCommand(track, pos, heading);
    command->boat()->setDim(64);
    m_undoStack->beginMacro("");
    m_undoStack->push(command);
    return command->boat();
}

MarkModel *SituationModel::createMark(QPointF pos) {
    m_undoStack->endMacro();
    AddMarkUndoCommand *command = new AddMarkUndoCommand(this, pos);
    m_undoStack->beginMacro("");
    m_undoStack->push(command);
    return command->mark();
}

PolyLineModel *SituationModel::createLine(QPointF pos) {
    AddPolyLineUndoCommand *command = new AddPolyLineUndoCommand(this);
    m_undoStack->push(command);
    PointModel *point = new PointModel(command->polyLine());
    point->setPosition(pos);
    command->polyLine()->addPoint(point);
    return command->polyLine();
}

PointModel *SituationModel::createPoint(PolyLineModel *poly, QPointF pos) {
    m_undoStack->endMacro();
    AddPointUndoCommand *command = new AddPointUndoCommand(poly, pos);
    m_undoStack->beginMacro("");
    m_undoStack->push(command);
    return command->point();
}

void SituationModel::clearSelectedModels() {
    m_selectedModels.clear();
    m_selectedBoatModels.clear();
    m_selectedMarkModels.clear();
    m_selectedPointModels.clear();
}

void SituationModel::addSelectedBoat(BoatModel *boat) {
    m_selectedBoatModels << boat;
    m_selectedModels << boat;
}

void SituationModel::addSelectedMark(MarkModel *mark) {
    m_selectedMarkModels << mark;
    m_selectedModels << mark;
}

void SituationModel::addSelectedPoint(PointModel *point) {
    m_selectedPointModels << point;
    m_selectedModels << point;
}

void SituationModel::addSelectedModel(PositionModel *position) {
    m_selectedModels << position;
}
