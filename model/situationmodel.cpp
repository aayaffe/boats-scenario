//
// C++ Implementation: situationmodel
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2014 Thibaut GRIDEL
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
#include "statemachine.h"
#include "scenarioanimation.h"

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
        m_undoStack(new QUndoStack(this)),
        m_stateMachine(new StateMachine(this)),
        m_scenarioAnimation(new ScenarioAnimation(this, this)) {
    if (debugLevel & 1 << MODEL) std::cout << "new situation " << this << std::endl;
    connect(&m_wind, SIGNAL(windReset()),
            this, SLOT(resetWind()));

    connect(m_undoStack, SIGNAL(canUndoChanged(bool)),
            this, SIGNAL(canUndoChanged(bool)));
    connect(m_undoStack, SIGNAL(canRedoChanged(bool)),
            this, SIGNAL(canRedoChanged(bool)));

    connect(m_stateMachine->animationState(), SIGNAL(entered()),
            m_scenarioAnimation, SLOT(setAnimation()));
    connect(m_stateMachine->animationState(), SIGNAL(exited()),
            m_scenarioAnimation, SLOT(unsetAnimation()));

    connect(m_stateMachine->createTrackState(), SIGNAL(entered()),
            this, SLOT(createTrack()));
    connect(m_stateMachine->createBoatState(), SIGNAL(entered()),
            this, SLOT(createBoat()));
    connect(m_stateMachine->createMarkState(), SIGNAL(entered()),
            this, SLOT(createMark()));
    connect(m_stateMachine->createLineState(), SIGNAL(entered()),
            this, SLOT(createLine()));
    connect(m_stateMachine->createPointState(), SIGNAL(entered()),
            this, SLOT(createPoint()));
    connect(m_stateMachine->moveState(), SIGNAL(entered()),
            this, SLOT(moveModel()));
    connect(m_stateMachine->rotateState(), SIGNAL(entered()),
            this, SLOT(rotateModel()));
    connect(m_stateMachine->createState(), SIGNAL(exited()),
            this, SLOT(exitCreateState()));
    m_stateMachine->start();
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
    emit tracksChanged();
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
    emit tracksChanged();
}

void SituationModel::addMark(MarkModel *mark, int order) {
    if (order == -1) {
        order = m_marks.size();
    }
    m_marks.insert(order, mark);
    connect(&m_wind, SIGNAL(directionChanged(qreal)),
            mark, SLOT(setWind(qreal)));
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

void SituationModel::changeTitle(QString title) {
    if (title != m_title) {
        m_undoStack->push(new SetTitleUndoCommand(this, title));
    }
}

void SituationModel::changeRules(QString rules) {
    if (rules != m_rules) {
        m_undoStack->push(new SetRulesUndoCommand(this, rules));
    }
}

void SituationModel::toggleShowLayline(bool showlayline) {
    if (showlayline != m_showLayline) {
        m_undoStack->push(new SetShowLaylineUndoCommand(this));
    }
}

void SituationModel::changeSeries(Boats::Series series) {
    if (series != m_situationSeries) {
        m_undoStack->push(new SetSituationSeriesUndoCommand(this, series));
    }
}

void SituationModel::changeLaylineAngle(int angle) {
    if (angle != m_laylineAngle) {
        m_undoStack->push(new SetLaylineUndoCommand(this, angle));
    }
}

void SituationModel::changeLength(int length) {
    if (length != m_situationLength) {
        m_undoStack->push(new LengthMarkUndoCommand(this, length));
    }
}

void SituationModel::changeAbstract(QString abstract) {
    if (abstract != m_abstract) {
        m_undoStack->push(new SetAbstractUndoCommand(this, abstract));
    }
}

void SituationModel::changeDescription(QString description) {
    if (description != m_description) {
        m_undoStack->push(new SetDescriptionUndoCommand(this, description));
    }
}

void SituationModel::toggleWind() {
    m_undoStack->push(new SetShowWindUndoCommand(&m_wind));
}

void SituationModel::moveModel() {
    if (!m_selectedModels.isEmpty()) {
        m_undoStack->push(new MoveModelUndoCommand(m_selectedModels, m_curPosition - m_selectedModels.first()->position()));
    }
    if (!m_selectedBoatModels.isEmpty()) {
        BoatModel *boat = m_selectedBoatModels[0];
        TrackModel *track = boat->track();
        if(boat->order() > 1) {
            qreal heading = track->headingForNext(
                        boat->order()-2, boat->position());
            boat->setHeading(heading);
        }
    }
}

void SituationModel::rotateModel() {

    if (!m_selectedModels.isEmpty()) {
        QPointF pos = m_curPosition - m_selectedModels.last()->position();
        if (pos == QPointF()) {
            return;
        }
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

void SituationModel::rotateModel(qreal angle) {
    m_undoStack->push(new RotateModelsUndoCommand(m_selectedModels, angle));
}

void SituationModel::deleteModels() {
    foreach (BoatModel *boat, m_selectedBoatModels) {
        TrackModel* track = boat->track();
        if (track->size() > 1) {
            m_undoStack->push(new DeleteBoatUndoCommand(track, boat));
        } else {
            m_undoStack->push(new DeleteTrackUndoCommand(this, track));
        }
    }
    foreach (MarkModel *mark, m_selectedMarkModels) {
        m_undoStack->push(new DeleteMarkUndoCommand(this, mark));
    }

    foreach (PointModel *point, m_selectedPointModels) {
        PolyLineModel *polyLine = point->polyLine();
        if (polyLine->size() > 1) {
            m_undoStack->push(new DeletePointUndoCommand(polyLine, point));
        } else {
            m_undoStack->push(new DeletePolyLineUndoCommand(this, polyLine));
        }
    }
}

void SituationModel::createTrack() {
    AddTrackUndoCommand *command = new AddTrackUndoCommand(this);
    m_undoStack->push(command);
    TrackModel *track = command->track();
    BoatModel *boat = new BoatModel(track, track);
    boat->setDim(64);
    boat->setPosition(m_curPosition);
    track->addBoat(boat);
    clearSelectedModels();
    addSelectedBoat(boat);
}

void SituationModel::createBoat() {
    if(!selectedBoatModels().isEmpty()) {
        TrackModel *track = m_selectedBoatModels[0]->track();
        m_undoStack->endMacro();
        track->boats().last()->setDim(255);
        qreal heading = track->boats().last()->heading();
        AddBoatUndoCommand *command = new AddBoatUndoCommand(track, m_curPosition, heading);
        command->boat()->setDim(64);
        m_undoStack->beginMacro("");
        m_undoStack->push(command);
        clearSelectedModels();
        addSelectedBoat(command->boat());
    }
}

void SituationModel::deleteTrack() {
    // TODO trick to delete first selected track
    if (!selectedBoatModels().isEmpty()) {
        BoatModel *boat = selectedBoatModels()[0];
        TrackModel * track = boat->track();
        m_undoStack->push(new DeleteTrackUndoCommand(this, track));
    }
}

void SituationModel::createMark() {
    m_undoStack->endMacro();
    AddMarkUndoCommand *command = new AddMarkUndoCommand(this, m_curPosition);
    m_undoStack->beginMacro("");
    m_undoStack->push(command);
    clearSelectedModels();
    addSelectedMark(command->mark());
}

void SituationModel::createLine() {
    AddPolyLineUndoCommand *command = new AddPolyLineUndoCommand(this);
    m_undoStack->push(command);
    PointModel *point = new PointModel(command->polyLine());
    point->setPosition(m_curPosition);
    command->polyLine()->addPoint(point);
    clearSelectedModels();
    addSelectedPoint(point);
}

void SituationModel::createPoint() {
    if(!selectedPointModels().isEmpty()) {
        PolyLineModel *poly = m_selectedPointModels[0]->polyLine();
        m_undoStack->endMacro();
        AddPointUndoCommand *command = new AddPointUndoCommand(poly, m_curPosition);
        m_undoStack->beginMacro("");
        m_undoStack->push(command);
        clearSelectedModels();
        addSelectedPoint(command->point());
    }
}

void SituationModel::setCurPosition(QPointF pos) {
    m_curPosition = pos;
}

void SituationModel::exitCreateState() {
    m_undoStack->endMacro();
    m_undoStack->undo();
}


void SituationModel::setColor(QColor color) {
    if (!m_selectedBoatModels.isEmpty()) {
        TrackModel *track = m_selectedBoatModels.first()->track();
        m_undoStack->push(new SetColorUndoCommand(track, color));
    }
}

void SituationModel::setShowPath() {
    if (!m_selectedBoatModels.isEmpty()) {
        TrackModel *track = m_selectedBoatModels.first()->track();
        m_undoStack->push(new SetShowPathUndoCommand(track));
    }
}

void SituationModel::setSeries(Boats::Series series) {
    if (!m_selectedBoatModels.isEmpty()) {
        TrackModel *track = m_selectedBoatModels.first()->track();
        m_undoStack->push(new SetSeriesUndoCommand(track, series));
    }
}

void SituationModel::setFollowTrack() {
    if (!m_selectedBoatModels.isEmpty()) {
        TrackModel *track = m_selectedBoatModels.first()->track();
        m_undoStack->push(new SetFollowTrackUndoCommand(this, track));
    }
}
void SituationModel::addWind(qreal wind) {
    m_undoStack->push(new AddWindUndoCommand(&m_wind, wind));
}

void SituationModel::setWind(int index, qreal wind) {
    m_undoStack->push(new SetWindUndoCommand(&m_wind, index, wind));
}

void SituationModel::deleteWind(int index) {
    m_undoStack->push(new DeleteWindUndoCommand(&m_wind, index));
}

void SituationModel::trimSail() {
    if (! m_selectedBoatModels.isEmpty()) {
        qreal trim = m_selectedBoatModels[0]->trim();
        qreal heading = fmod(m_selectedBoatModels[0]->heading() - m_selectedBoatModels[0]->wind() + 360, 360);
        if(heading < 0) heading +=360;
        if (heading < 180) {
            trim -= 5;
        } else {
            trim += 5;
        }
        m_undoStack->push(new TrimBoatUndoCommand(m_selectedBoatModels, trim));
    }
}

void SituationModel::autotrimSail() {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new TrimBoatUndoCommand(m_selectedBoatModels, 0));
    }
}

void SituationModel::untrimSail() {
    if (! m_selectedBoatModels.isEmpty()) {
        qreal trim = m_selectedBoatModels[0]->trim();
        qreal heading = fmod(m_selectedBoatModels[0]->heading() - m_selectedBoatModels[0]->wind() + 360, 360);
        if(heading < 0) heading +=360;
        if (heading < 180) {
            trim += 5;
        } else {
            trim -= 5;
        }
        m_undoStack->push(new TrimBoatUndoCommand(m_selectedBoatModels, trim));
    }
}

void SituationModel::trimJib() {
    if (! m_selectedBoatModels.isEmpty()) {
        qreal trim = m_selectedBoatModels[0]->jibTrim();
        qreal heading = fmod(m_selectedBoatModels[0]->heading() - m_selectedBoatModels[0]->wind() + 360, 360);
        if(heading < 0) heading +=360;
        if (heading < 180) {
            trim -= 5;
        } else {
            trim += 5;
        }
        m_undoStack->push(new TrimJibUndoCommand(m_selectedBoatModels, trim));
    }
}

void SituationModel::autotrimJib() {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new TrimJibUndoCommand(m_selectedBoatModels, 0));
    }
}

void SituationModel::untrimJib() {
    if (! m_selectedBoatModels.isEmpty()) {
        qreal trim = m_selectedBoatModels[0]->jibTrim();
        qreal heading = fmod(m_selectedBoatModels[0]->heading() - m_selectedBoatModels[0]->wind() + 360, 360);
        if(heading < 0) heading +=360;
        if (heading < 180) {
            trim += 5;
        } else {
            trim -= 5;
        }
        m_undoStack->push(new TrimJibUndoCommand(m_selectedBoatModels, trim));
    }
}

void SituationModel::trimSpin() {
    if (! m_selectedBoatModels.isEmpty()) {
        qreal trim = m_selectedBoatModels[0]->spinTrim();
        qreal heading = fmod(m_selectedBoatModels[0]->heading() - m_selectedBoatModels[0]->wind() + 360, 360);
        if(heading < 0) heading +=360;
        if (heading < 180) {
            trim -= 5;
        } else {
            trim += 5;
        }
        m_undoStack->push(new TrimSpinUndoCommand(m_selectedBoatModels, trim));
    }
}

void SituationModel::autotrimSpin() {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new TrimSpinUndoCommand(m_selectedBoatModels, 0));
    }
}

void SituationModel::untrimSpin() {
    if (! m_selectedBoatModels.isEmpty()) {
        qreal trim = m_selectedBoatModels[0]->spinTrim();
        qreal heading = fmod(m_selectedBoatModels[0]->heading() - m_selectedBoatModels[0]->wind() + 360, 360);
        if(heading < 0) heading +=360;
        if (heading < 180) {
            trim += 5;
        } else {
            trim -= 5;
        }
        m_undoStack->push(new TrimSpinUndoCommand(m_selectedBoatModels, trim));
    }
}

void SituationModel::togglePortOverlap() {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new OverlapBoatUndoCommand(this, m_selectedBoatModels, Boats::port));
    }
}

void SituationModel::toggleStarboardOverlap() {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new OverlapBoatUndoCommand(this, m_selectedBoatModels, Boats::starboard));
    }
}

void SituationModel::toggleHidden() {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new HiddenBoatUndoCommand(this, m_selectedBoatModels, !m_selectedBoatModels.first()->hidden()));
    }
}

void SituationModel::toggleText() {
    if (! m_selectedModels.isEmpty()) {
        QString text;
        if (m_selectedModels.first()->text().isEmpty()) {
            text = tr("Protest!");
        }
        m_undoStack->push(new SetTextUndoCommand(m_selectedModels.first(), text));
    }
}

void SituationModel::setText(QString text) {
    if (! m_selectedModels.isEmpty()) {
        m_undoStack->push(new SetTextUndoCommand(m_selectedModels.first(), text));
    }
}

void SituationModel::moveText(QPointF pos) {
    if (! m_selectedModels.isEmpty()) {
        PositionModel *model = m_selectedModels.first();
        m_undoStack->push(new MoveTextUndoCommand(model, pos - model->textPosition()));
    }
}

void SituationModel::toggleFlag(Boats::Flag flag) {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new FlagBoatUndoCommand(this, m_selectedBoatModels, flag));
    }
}

void SituationModel::toggleAcceleration(Boats::Acceleration acceleration) {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new AccelerateBoatUndoCommand(this, m_selectedBoatModels, acceleration));
    }
}

void SituationModel::toggleSpin() {
    if (! m_selectedBoatModels.isEmpty()) {
        m_undoStack->push(new SpinBoatUndoCommand(this, m_selectedBoatModels, !m_selectedBoatModels.first()->spin()));
    }
}

void SituationModel::toggleMarkSide() {
    if (! m_selectedMarkModels.isEmpty()) {
        m_undoStack->push(new ToggleMarkSideUndoCommand(m_selectedMarkModels));
    } else {
        m_undoStack->push(new ToggleMarkSideUndoCommand(m_marks));
    }
}

void SituationModel::toggleMarkArrow() {
    if (! m_selectedMarkModels.isEmpty()) {
        m_undoStack->push(new ToggleMarkArrowUndoCommand(m_selectedMarkModels));
    } else {
        m_undoStack->push(new ToggleMarkArrowUndoCommand(m_marks));
    }
}

void SituationModel::toggleMarkZone() {
    if (! m_selectedMarkModels.isEmpty()) {
        m_undoStack->push(new ZoneMarkUndoCommand(this, m_selectedMarkModels));
    } else {
        m_undoStack->push(new ZoneMarkUndoCommand(this, m_marks));
    }
}

void SituationModel::setMarkColor(QColor color) {
    if (! m_selectedMarkModels.isEmpty()) {
        m_undoStack->push(new ColorMarkUndoCommand(this, m_selectedMarkModels, color));
    }
}

void SituationModel::toggleMarkLabel() {
    if (! m_selectedMarkModels.isEmpty()) {
        m_undoStack->push(new ToggleMarkLabelUndoCommand(m_selectedMarkModels));
    } else {
        m_undoStack->push(new ToggleMarkLabelUndoCommand(m_marks));
    }
}

void SituationModel::editMarkLabel(QString text) {
    if (! m_selectedMarkModels.isEmpty()) {
        m_undoStack->push(new SetMarkLabelUndoCommand(m_selectedMarkModels.first(), text));
    }
}

void SituationModel::toggleLaylines() {
    if (! m_selectedModels.isEmpty()) {
        m_undoStack->push(new SetLaylinesUndoCommand(m_selectedModels, !m_selectedModels.first()->laylines()));
    }
}

void SituationModel::setLookAt(int direction, int tilt) {
    m_undoStack->push(new SetLookAtUndoCommand(this, direction, tilt));
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
    m_stateMachine->selectBoat();
}

void SituationModel::addSelectedMark(MarkModel *mark) {
    m_selectedMarkModels << mark;
    m_selectedModels << mark;
}

void SituationModel::addSelectedPoint(PointModel *point) {
    m_selectedPointModels << point;
    m_selectedModels << point;
    m_stateMachine->selectPoint();
}

void SituationModel::addSelectedModel(PositionModel *position) {
    m_selectedModels << position;
}

void SituationModel::removeSelectedModel(PositionModel *position) {
    m_selectedModels.removeOne(position);
    m_selectedBoatModels.removeOne(qobject_cast<BoatModel*>(position));
    m_selectedMarkModels.removeOne(qobject_cast<MarkModel*>(position));
    m_selectedPointModels.removeOne(qobject_cast<PointModel*>(position));
}
