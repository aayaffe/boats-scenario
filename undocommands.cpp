//
// C++ Implementation: undocommands
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
#include "undocommands.h"

#include <iostream>
#include <cmath>

#include "commontypes.h"
#include "boats.h"

#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"

extern int debugLevel;

// Set Title
SetTitleUndoCommand::SetTitleUndoCommand(SituationModel* situation, QString title, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_oldTitle(situation->title()),
        m_newTitle(title) {
    if (debugLevel & 1 << COMMAND) std::cout << "new settitleundocommand" << std::endl;
}

SetTitleUndoCommand::~SetTitleUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end settitleundocommand" << std::endl;
}

void SetTitleUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo settitleundocommand"<< std::endl;
    m_situation->setTitle(m_oldTitle);
}

void SetTitleUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo settitleundocommand" << std::endl;
    m_situation->setTitle(m_newTitle);
}

bool SetTitleUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetTitleUndoCommand *settitleCommand = static_cast<const SetTitleUndoCommand*>(command);
    if (m_situation != settitleCommand->m_situation)
        return false;

    m_newTitle = settitleCommand->m_newTitle;
    return true;
}

// Set Rules
SetRulesUndoCommand::SetRulesUndoCommand(SituationModel* situation, QString rules, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_oldRules(situation->rules()),
        m_newRules(rules) {
    if (debugLevel & 1 << COMMAND) std::cout << "new setrulesundocommand" << std::endl;
}

SetRulesUndoCommand::~SetRulesUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end setrulesundocommand" << std::endl;
}

void SetRulesUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo setrulesundocommand"<< std::endl;
    m_situation->setRules(m_oldRules);
}

void SetRulesUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo setrulesundocommand" << std::endl;
    m_situation->setRules(m_newRules);
}

bool SetRulesUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetRulesUndoCommand *setrulesCommand = static_cast<const SetRulesUndoCommand*>(command);
    if (m_situation != setrulesCommand->m_situation)
        return false;

    m_newRules = setrulesCommand->m_newRules;
    return true;
}

// Show Layline
SetShowLaylineUndoCommand::SetShowLaylineUndoCommand(SituationModel *situation, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation) {
    if (debugLevel & 1 << COMMAND) std::cout << "new showlaylineundocommand" << std::endl;
}

SetShowLaylineUndoCommand::~SetShowLaylineUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end showlaylineundocommand" << std::endl;
}

void SetShowLaylineUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo showlaylineundocommand" << std::endl;
    m_situation->setShowLayline(!m_situation->showLayline());
}

void SetShowLaylineUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo showlaylineundocommand" << std::endl;
    m_situation->setShowLayline(!m_situation->showLayline());
}

bool SetShowLaylineUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetShowLaylineUndoCommand *zoneCommand = static_cast<const SetShowLaylineUndoCommand*>(command);
    if (m_situation != zoneCommand->m_situation)
        return false;
    undo();
    m_situation->undoStack()->setIndex(m_situation->undoStack()->index()-1);
    return true;
}

// Set Layline
SetLaylineUndoCommand::SetLaylineUndoCommand(SituationModel* situation, const int angle, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_oldAngle(situation->laylineAngle()),
        m_newAngle(angle) {
    if (debugLevel & 1 << COMMAND) std::cout << "new setlaylineundocommand" << std::endl;
}

SetLaylineUndoCommand::~SetLaylineUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end setlaylineundocommand" << std::endl;
}

void SetLaylineUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo setlaylineundocommand" << std::endl;
    m_situation->setLaylineAngle(m_oldAngle);
}

void SetLaylineUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo setlaylineundocommand" << std::endl;
    m_situation->setLaylineAngle(m_newAngle);
}

bool SetLaylineUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetLaylineUndoCommand *setlaylineCommand = static_cast<const SetLaylineUndoCommand*>(command);
    if (m_situation != setlaylineCommand->m_situation)
        return false;

    m_newAngle = setlaylineCommand->m_newAngle;
    return true;
}

// Set Series
SetSituationSeriesUndoCommand::SetSituationSeriesUndoCommand(SituationModel* situation, const int series, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_oldSeries(situation->situationSeries()),
        m_newSeries(series) {
    if (debugLevel & 1 << COMMAND) std::cout << "new SetSituationSeriesUndoCommand" << std::endl;
}

SetSituationSeriesUndoCommand::~SetSituationSeriesUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end SetSituationSeriesUndoCommand" << std::endl;
}

void SetSituationSeriesUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo SetSituationSeriesUndoCommand" << std::endl;
    m_situation->setSituationSeries(m_oldSeries);
}

void SetSituationSeriesUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo SetSituationSeriesUndoCommand" << std::endl;
    m_situation->setSituationSeries(m_newSeries);
}

bool SetSituationSeriesUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetSituationSeriesUndoCommand *setseriesCommand = static_cast<const SetSituationSeriesUndoCommand*>(command);
    if (m_situation != setseriesCommand->m_situation)
        return false;

    m_newSeries = setseriesCommand->m_newSeries;
    return true;
}

// Set Abstract
SetAbstractUndoCommand::SetAbstractUndoCommand(SituationModel* situation, QString abstract, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_oldAbstract(situation->abstract()),
        m_newAbstract(abstract) {
    if (debugLevel & 1 << COMMAND) std::cout << "new setabstractundocommand" << std::endl;
}

SetAbstractUndoCommand::~SetAbstractUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end setabstractundocommand" << std::endl;
}

void SetAbstractUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo setabstractundocommand"<< std::endl;
    m_situation->setAbstract(m_oldAbstract);
}

void SetAbstractUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo setabstractundocommand" << std::endl;
    m_situation->setAbstract(m_newAbstract);
}

bool SetAbstractUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetAbstractUndoCommand *setabstractCommand = static_cast<const SetAbstractUndoCommand*>(command);
    if (m_situation != setabstractCommand->m_situation)
        return false;

    m_newAbstract = setabstractCommand->m_newAbstract;
    return true;
}

// Set Description
SetDescriptionUndoCommand::SetDescriptionUndoCommand(SituationModel* situation, QString description, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_oldDescription(situation->description()),
        m_newDescription(description) {
    if (debugLevel & 1 << COMMAND) std::cout << "new setdescriptionundocommand" << std::endl;
}

SetDescriptionUndoCommand::~SetDescriptionUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end setdescriptionundocommand" << std::endl;
}

void SetDescriptionUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo setdescriptionundocommand"<< std::endl;
    m_situation->setDescription(m_oldDescription);
}

void SetDescriptionUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo setdescriptionundocommand" << std::endl;
    m_situation->setDescription(m_newDescription);
}

bool SetDescriptionUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetDescriptionUndoCommand *setdescriptionCommand = static_cast<const SetDescriptionUndoCommand*>(command);
    if (m_situation != setdescriptionCommand->m_situation)
        return false;

    m_newDescription = setdescriptionCommand->m_newDescription;
    return true;
}

// Add Track
AddTrackUndoCommand::AddTrackUndoCommand(SituationModel* situation, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation) {
    if (debugLevel & 1 << COMMAND) std::cout << "new addtrackundocommand" << std::endl;
    m_track = new TrackModel(situation);
}

AddTrackUndoCommand::~AddTrackUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end addtrackundocommand" << std::endl;
    delete m_track;
}

void AddTrackUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo addtrackundocommand" << std::endl;
    m_situation->addTrack(m_track);
}

void AddTrackUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo addtrackundocommand" << std::endl;
    m_situation->deleteTrack(m_track);
}

// Delete Track
DeleteTrackUndoCommand::DeleteTrackUndoCommand(SituationModel* situation, TrackModel* track, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_track(track) {
    if (debugLevel & 1 << COMMAND) std::cout << "new removetrackundocommand" << std::endl;
}

DeleteTrackUndoCommand::~DeleteTrackUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end removetrackundocommand" << std::endl;
}

void DeleteTrackUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo removetrackundocommand" << std::endl;
    m_situation->deleteTrack(m_track);
}

void DeleteTrackUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo removetrackundocommand" << std::endl;
    m_situation->addTrack(m_track);
}

// Set Series
SetSeriesUndoCommand::SetSeriesUndoCommand(TrackModel* track, const Boats::Series series, QUndoCommand *parent)
    : QUndoCommand(parent),
    m_track(track),
    m_oldSeries(track->series()),
    m_newSeries(series) {
    if (debugLevel & 1 << COMMAND) std::cout << "new setseriesundocommand" << std::endl;
}

SetSeriesUndoCommand::~SetSeriesUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end setseriesundocommand" << std::endl;
}

void SetSeriesUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo setseriesundocommand" << std::endl;
    m_track->setSeries(m_oldSeries);
}

void SetSeriesUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo setseriesundocommand" << std::endl;
    m_track->setSeries(m_newSeries);
}

bool SetSeriesUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetSeriesUndoCommand *seriesCommand = static_cast<const SetSeriesUndoCommand*>(command);
    if (m_track != seriesCommand->m_track)
        return false;

    m_newSeries = seriesCommand->m_newSeries;
    return true;
}

// Set Color
SetColorUndoCommand::SetColorUndoCommand(TrackModel* track, const QColor color, QUndoCommand *parent)
    : QUndoCommand(parent),
    m_track(track),
    m_oldColor(track->color()),
    m_newColor(color) {
    if (debugLevel & 1 << COMMAND) std::cout << "new setcolorundocommand" << std::endl;
}

SetColorUndoCommand::~SetColorUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end setcolorundocommand" << std::endl;
}

void SetColorUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo setcolorundocommand" << std::endl;
    m_track->setColor(m_oldColor);
}

void SetColorUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo setcolorundocommand" << std::endl;
    m_track->setColor(m_newColor);
}

bool SetColorUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetColorUndoCommand *colorCommand = static_cast<const SetColorUndoCommand*>(command);
    if (m_track != colorCommand->m_track)
        return false;

    m_newColor = colorCommand->m_newColor;
    return true;
}

// Show Path
SetShowPathUndoCommand::SetShowPathUndoCommand(TrackModel *track, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_track(track) {
    if (debugLevel & 1 << COMMAND) std::cout << "new showlaylineundocommand" << std::endl;
}

SetShowPathUndoCommand::~SetShowPathUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end showlaylineundocommand" << std::endl;
}

void SetShowPathUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo showlaylineundocommand" << std::endl;
    m_track->setShowPath(!m_track->showPath());
}

void SetShowPathUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo showlaylineundocommand" << std::endl;
    m_track->setShowPath(!m_track->showPath());
}

bool SetShowPathUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetShowPathUndoCommand *showPathCommand = static_cast<const SetShowPathUndoCommand*>(command);
    if (m_track != showPathCommand->m_track)
        return false;
    undo();
    m_track->situation()->undoStack()->setIndex(m_track->situation()->undoStack()->index()-1);
    return true;
}

// Follow Track
SetFollowTrackUndoCommand::SetFollowTrackUndoCommand(SituationModel* situation, TrackModel *track, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_track(track) {
    if (debugLevel & 1 << COMMAND) std::cout << "new followtrackundocommand" << std::endl;
    foreach (TrackModel *tracks, situation->tracks()) {
        m_followTrackList.append(tracks->followTrack());
    }
}

SetFollowTrackUndoCommand::~SetFollowTrackUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end followtrackundocommand" << std::endl;
}

void SetFollowTrackUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo followtrackundocommand" << std::endl;
    for (int i=0; i<m_situation->size(); ++i) {
        TrackModel *tracks = m_situation->tracks()[i];
        tracks->setFollowTrack(m_followTrackList.at(i));
    }
}

void SetFollowTrackUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo followtrackundocommand" << std::endl;
    m_track->setFollowTrack(!m_track->followTrack());
}

bool SetFollowTrackUndoCommand::mergeWith(const QUndoCommand *command) {
    return false;
}

// Lookat
SetLookAtUndoCommand::SetLookAtUndoCommand(SituationModel* situation, int lookDirection, int tilt, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_oldLookDirection(situation->lookDirection()),
        m_newLookDirection(lookDirection),
        m_oldTilt(situation->tilt()),
        m_newTilt(tilt) {
    if (debugLevel & 1 << COMMAND) std::cout << "new lookatundocommand" << std::endl;
}

SetLookAtUndoCommand::~SetLookAtUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end lookatundocommand" << std::endl;
}

void SetLookAtUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo lookatundocommand" << std::endl;
    m_situation->setLookDirection(m_oldLookDirection);
    m_situation->setTilt(m_oldTilt);
}

void SetLookAtUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo lookatundocommand" << std::endl;
    m_situation->setLookDirection(m_newLookDirection);
    m_situation->setTilt(m_newTilt);
}

bool SetLookAtUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetLookAtUndoCommand *lookatCommand = static_cast<const SetLookAtUndoCommand*>(command);
    if (m_situation != lookatCommand->m_situation)
        return false;
    m_newLookDirection = lookatCommand->m_newLookDirection;
    m_newTilt = lookatCommand->m_newTilt;
    return true;
}

// Show Wind
SetShowWindUndoCommand::SetShowWindUndoCommand(WindModel *wind, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_wind(wind) {
    if (debugLevel & 1 << COMMAND) std::cout << "new showwindundocommand" << std::endl;
}

SetShowWindUndoCommand::~SetShowWindUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end showwindundocommand" << std::endl;
}

void SetShowWindUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo showwindundocommand" << std::endl;
    m_wind->setVisible(!m_wind->visible());
}

void SetShowWindUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo showwindundocommand" << std::endl;
    m_wind->setVisible(!m_wind->visible());
}

bool SetShowWindUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetShowWindUndoCommand *visibleCommand = static_cast<const SetShowWindUndoCommand*>(command);
    if (m_wind != visibleCommand->m_wind)
        return false;
    undo();
    m_wind->situation()->undoStack()->setIndex(m_wind->situation()->undoStack()->index()-1);
    return true;
}

// Add Wind
AddWindUndoCommand::AddWindUndoCommand(WindModel* wind, qreal heading, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_wind(wind),
        m_heading(heading) {
    if (debugLevel & 1 << COMMAND) std::cout << "new addwindundocommand" << std::endl;
}

AddWindUndoCommand::~AddWindUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end addwindundocommand" << std::endl;
}

void AddWindUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo addwindundocommand" << std::endl;
    m_wind->addWind(m_heading);
}

void AddWindUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo addwindundocommand" << std::endl;
    m_wind->deleteWind(m_wind->size()-1);
}

// Set Wind
SetWindUndoCommand::SetWindUndoCommand(WindModel* wind, int index, qreal direction, QUndoCommand *parent)
    : QUndoCommand(parent),
    m_wind(wind),
    m_index(index),
    m_oldDirection(wind->windAt(index)),
    m_newDirection(direction) {
    if (debugLevel & 1 << COMMAND) std::cout << "new setwindundocommand" << std::endl;
}

SetWindUndoCommand::~SetWindUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end setwindundocommand" << std::endl;
}

void SetWindUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo setwindundocommand" << std::endl;
    m_wind->setWindAt(m_oldDirection, m_index);
}

void SetWindUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo setwindundocommand" << std::endl;
    m_wind->setWindAt(m_newDirection, m_index);
}

bool SetWindUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetWindUndoCommand *setWindCommand = static_cast<const SetWindUndoCommand*>(command);
    if (m_index != setWindCommand->m_index)
        return false;

    m_newDirection = setWindCommand->m_newDirection;
    return true;
}

// Delete Wind
DeleteWindUndoCommand::DeleteWindUndoCommand(WindModel* wind, int index, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_wind(wind),
        m_index(index),
        m_heading(m_wind->windAt(index)) {
    if (debugLevel & 1 << COMMAND) std::cout << "new deletewindundocommand" << std::endl;
}

DeleteWindUndoCommand::~DeleteWindUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end deletewindundocommand" << std::endl;
}

void DeleteWindUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo deletewindundocommand" << std::endl;
    m_wind->deleteWind(m_index);
}

void DeleteWindUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo deletewindundocommand" << std::endl;
    m_wind->addWind(m_heading, m_index);
}

// Move Model
MoveModelUndoCommand::MoveModelUndoCommand(QList<PositionModel*> &modelList, const QPointF &deltaPosition, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_modelList(modelList),
        m_deltaPosition(deltaPosition) {
    if (debugLevel & 1 << COMMAND) std::cout << "new movemodelundocommand" << std::endl;
}

MoveModelUndoCommand::~MoveModelUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end movemodelundocommand" << std::endl;
}

void MoveModelUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo movemodelundocommand" << std::endl;
    for(int i=0; i< m_modelList.size(); i++) {
        PositionModel *model = m_modelList[i];
        model->setPosition(model->position() - m_deltaPosition);
    }
}

void MoveModelUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo movemodelundocommand" << std::endl;
    for(int i=0; i< m_modelList.size(); i++) {
        PositionModel *model = m_modelList[i];
        model->setPosition(model->position() + m_deltaPosition);
    }
}

bool MoveModelUndoCommand::mergeWith(const QUndoCommand *command) {
    const MoveModelUndoCommand *moveCommand = static_cast<const MoveModelUndoCommand*>(command);
    if (m_modelList != moveCommand->m_modelList)
        return false;

    m_deltaPosition += moveCommand->m_deltaPosition;
    return true;
}

// Laylines Boat
SetLaylinesUndoCommand::SetLaylinesUndoCommand(QList<PositionModel*> &modelList, bool laylines, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_modelList(modelList),
        m_laylines(laylines) {
    if (debugLevel & 1 << COMMAND) std::cout << "new laylinesboatundocommand" << std::endl;
    foreach (const PositionModel *model, modelList) {
        m_laylinesList << model->laylines();
    }
}

SetLaylinesUndoCommand::~SetLaylinesUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end laylinesboatundocommand" << std::endl;
}

void SetLaylinesUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo laylinesboatundocommand" << std::endl;
    for(int i=0; i< m_modelList.size(); i++) {
        PositionModel *model = m_modelList[i];
        model->setLaylines(m_laylinesList[i]);
    }
}

void SetLaylinesUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo laylinesboatundocommand" << std::endl;
    for(int i=0; i< m_modelList.size(); i++) {
        PositionModel *model = m_modelList[i];
        model->setLaylines(m_laylines);
    }
}

bool SetLaylinesUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetLaylinesUndoCommand *laylinesCommand = static_cast<const SetLaylinesUndoCommand*>(command);
    if (m_modelList != laylinesCommand->m_modelList)
        return false;

    m_laylines = laylinesCommand->m_laylines;
    return true;
}

// Add Boat
AddBoatUndoCommand::AddBoatUndoCommand(TrackModel* track, QPointF& position, qreal heading, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_track(track) {
    if (debugLevel & 1 << COMMAND) std::cout << "new addboatundocommand" << std::endl;
    m_boat = new BoatModel(track);
    m_boat->setPosition(position);
    m_boat->setHeading(heading);
}

AddBoatUndoCommand::~AddBoatUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end addboatundocommand" << std::endl;
    delete m_boat;
}

void AddBoatUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo addboatundocommand" << std::endl;
    m_track->addBoat(m_boat);
}

void AddBoatUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo addboatundocommand" << std::endl;
    m_track->deleteBoat(m_boat);
}

// Rotate Boats
RotateBoatsUndoCommand::RotateBoatsUndoCommand(QList<BoatModel*> &boatList, const qreal &angle, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_boatList(boatList),
        m_angle(angle) {
    if (debugLevel & 1 << COMMAND) std::cout << "new rotateboatsundocommand" << std::endl;
    foreach (const BoatModel *boat, boatList) {
        m_headingList << boat->heading();
    }
}

RotateBoatsUndoCommand::~RotateBoatsUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end rotateboatsundocommand" << std::endl;
}

void RotateBoatsUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo rotateboatsundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setHeading(m_headingList[i]);
    }
}

void RotateBoatsUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo rotateboatsundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setHeading(boat->heading() + m_angle);
    }
}

bool RotateBoatsUndoCommand::mergeWith(const QUndoCommand *command) {
    const RotateBoatsUndoCommand *headingCommand = static_cast<const RotateBoatsUndoCommand*>(command);
    if (m_boatList != headingCommand->m_boatList)
        return false;

    m_angle += headingCommand->m_angle;
    return true;
}

// Overlap Boat
OverlapBoatUndoCommand::OverlapBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, Boats::Overlaps overlaps, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_boatList(boatList),
        m_overlaps(overlaps) {
    if (debugLevel & 1 << COMMAND) std::cout << "new overlapboatundocommand" << std::endl;
}

OverlapBoatUndoCommand::~OverlapBoatUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end overlapboatundocommand" << std::endl;
}

void OverlapBoatUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo overlapboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setOverlap(boat->overlap() ^ m_overlaps);
    }
}

void OverlapBoatUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo overlapboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setOverlap(boat->overlap() ^ m_overlaps);
    }
}

bool OverlapBoatUndoCommand::mergeWith(const QUndoCommand *command) {
    const OverlapBoatUndoCommand *overlapCommand = static_cast<const OverlapBoatUndoCommand*>(command);
    if (m_boatList != overlapCommand->m_boatList)
        return false;

    if ((m_overlaps ^ overlapCommand->m_overlaps) == Boats::none) {
        undo();
        m_situation->undoStack()->setIndex(m_situation->undoStack()->index()-1);
    } else {
        m_overlaps = m_overlaps ^ overlapCommand->m_overlaps;
    }
    return true;
}

// Flag Boat
FlagBoatUndoCommand::FlagBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, Boats::Flag flag, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_boatList(boatList),
        m_flag(flag) {
    if (debugLevel & 1 << COMMAND) std::cout << "new flagboatundocommand" << std::endl;
    foreach (const BoatModel *boat, boatList) {
        m_flagList << boat->flag();
    }
}

FlagBoatUndoCommand::~FlagBoatUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end flagboatundocommand" << std::endl;
}

void FlagBoatUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo flagboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setFlag(m_flagList[i]);
    }
}

void FlagBoatUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo flagboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setFlag(m_flag);
    }
}

bool FlagBoatUndoCommand::mergeWith(const QUndoCommand *command) {
    const FlagBoatUndoCommand *flagCommand = static_cast<const FlagBoatUndoCommand*>(command);
    if (m_boatList != flagCommand->m_boatList)
        return false;

    m_flag = flagCommand->m_flag;
    return true;
}

// Trim Boat
TrimBoatUndoCommand::TrimBoatUndoCommand(QList<BoatModel*> &boatList, const qreal &trim, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_boatList(boatList),
        m_trim(trim) {
    if (debugLevel & 1 << COMMAND) std::cout << "new trimboatundocommand" << std::endl;
    foreach (const BoatModel *boat, boatList) {
        m_trimList << boat->trim();
    }
}

TrimBoatUndoCommand::~TrimBoatUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end trimboatundocommand" << std::endl;
}

void TrimBoatUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo trimboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *model = m_boatList[i];
        model->setTrim(m_trimList[i]);
    }
}

void TrimBoatUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo trimboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *model = m_boatList[i];
        model->setTrim(m_trim);
    }
}

bool TrimBoatUndoCommand::mergeWith(const QUndoCommand *command) {
    const TrimBoatUndoCommand *moveCommand = static_cast<const TrimBoatUndoCommand*>(command);
    if (m_boatList != moveCommand->m_boatList)
        return false;

    m_trim = moveCommand->m_trim;
    return true;
}

// Spin Boat
SpinBoatUndoCommand::SpinBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, bool spin, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_boatList(boatList),
        m_spin(spin) {
    if (debugLevel & 1 << COMMAND) std::cout << "new spinboatundocommand" << std::endl;
    foreach (const BoatModel *boat, boatList) {
        m_spinList << boat->spin();
    }
}

SpinBoatUndoCommand::~SpinBoatUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end spinboatundocommand" << std::endl;
}

void SpinBoatUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo spinboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setSpin(m_spinList[i]);
    }
}

void SpinBoatUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo spinboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setSpin(m_spin);
    }
}

bool SpinBoatUndoCommand::mergeWith(const QUndoCommand *command) {
    const SpinBoatUndoCommand *spinCommand = static_cast<const SpinBoatUndoCommand*>(command);
    if (m_boatList != spinCommand->m_boatList)
        return false;

    m_spin = spinCommand->m_spin;
    return true;
}

// Hidden Boat
HiddenBoatUndoCommand::HiddenBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, bool hidden, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_boatList(boatList),
        m_hidden(hidden) {
    if (debugLevel & 1 << COMMAND) std::cout << "new hiddenboatundocommand" << std::endl;
    foreach (const BoatModel *boat, boatList) {
        m_hiddenList << boat->hidden();
    }
}

HiddenBoatUndoCommand::~HiddenBoatUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end hiddenboatundocommand" << std::endl;
}

void HiddenBoatUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo hiddenboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setHidden(m_hiddenList[i]);
    }
}

void HiddenBoatUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo hiddenboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setHidden(m_hidden);
    }
}

bool HiddenBoatUndoCommand::mergeWith(const QUndoCommand *command) {
    const HiddenBoatUndoCommand *hiddenCommand = static_cast<const HiddenBoatUndoCommand*>(command);
    if (m_boatList != hiddenCommand->m_boatList)
        return false;

    m_hidden = hiddenCommand->m_hidden;
    return true;
}

// Accelerate Boat
AccelerateBoatUndoCommand::AccelerateBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, Boats::Acceleration acceleration, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_boatList(boatList),
        m_acceleration(acceleration) {
    if (debugLevel & 1 << COMMAND) std::cout << "new accelerateboatundocommand" << std::endl;
    foreach (const BoatModel *boat, boatList) {
        m_accelerationList << boat->acceleration();
    }
}

AccelerateBoatUndoCommand::~AccelerateBoatUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end accelerateboatundocommand" << std::endl;
}

void AccelerateBoatUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo accelerateboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setAcceleration(m_accelerationList[i]);
    }
}

void AccelerateBoatUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo accelerateboatundocommand" << std::endl;
    for(int i=0; i< m_boatList.size(); i++) {
        BoatModel *boat = m_boatList[i];
        boat->setAcceleration(m_acceleration);
    }
}

bool AccelerateBoatUndoCommand::mergeWith(const QUndoCommand *command) {
    const AccelerateBoatUndoCommand *accelerationCommand = static_cast<const AccelerateBoatUndoCommand*>(command);
    if (m_boatList != accelerationCommand->m_boatList)
        return false;

    m_acceleration = accelerationCommand->m_acceleration;
    return true;
}

// Set Text
SetTextUndoCommand::SetTextUndoCommand(PositionModel* model, QString text, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_model(model),
        m_oldText(model->text()),
        m_newText(text) {
    if (debugLevel & 1 << COMMAND) std::cout << "new settextundocommand" << std::endl;
}

SetTextUndoCommand::~SetTextUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end settextundocommand" << std::endl;
}

void SetTextUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo settextundocommand"<< std::endl;
    m_model->setText(m_oldText);
}

void SetTextUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo settextundocommand" << std::endl;
    m_model->setText(m_newText);
}

bool SetTextUndoCommand::mergeWith(const QUndoCommand *command) {
    const SetTextUndoCommand *setTextCommand = static_cast<const SetTextUndoCommand*>(command);
    if (m_model != setTextCommand->m_model)
        return false;

    m_newText = setTextCommand->m_newText;
    return true;
}

// Move Text
MoveTextUndoCommand::MoveTextUndoCommand(PositionModel *model, const QPointF &deltaPosition, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_model(model),
        m_deltaPosition(deltaPosition) {
    if (debugLevel & 1 << COMMAND) std::cout << "new movetextundocommand" << std::endl;
}

MoveTextUndoCommand::~MoveTextUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end movetextundocommand" << std::endl;
}

void MoveTextUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo movetextundocommand" << std::endl;
    m_model->setTextPosition(m_model->textPosition() - m_deltaPosition);
}

void MoveTextUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo movetextundocommand" << std::endl;
    m_model->setTextPosition(m_model->textPosition() + m_deltaPosition);
}

bool MoveTextUndoCommand::mergeWith(const QUndoCommand *command) {
    const MoveTextUndoCommand *moveCommand = static_cast<const MoveTextUndoCommand*>(command);
    if (m_model != moveCommand->m_model)
        return false;

    m_deltaPosition += moveCommand->m_deltaPosition;
    return true;
}

// Delete Boat
DeleteBoatUndoCommand::DeleteBoatUndoCommand(TrackModel* track, BoatModel* boat, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_track(track),
        m_boat(boat) {
    if (debugLevel & 1 << COMMAND) std::cout << "new deleteboatundocommand" << std::endl;
}

DeleteBoatUndoCommand::~DeleteBoatUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end deletebboatundocommand" << std::endl;
}

void DeleteBoatUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo deletebboatundocommand" << std::endl;
    m_order = m_track->deleteBoat(m_boat);
}

void DeleteBoatUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo deletebboatundocommand" << std::endl;
    m_track->addBoat(m_boat, m_order);
}

// Add Mark
AddMarkUndoCommand::AddMarkUndoCommand(SituationModel* situation, QPointF& position, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation) {
    if (debugLevel & 1 << COMMAND) std::cout << "new addmarkundocommand" << std::endl;
    m_mark = new MarkModel(situation);
    m_mark->setPosition(position);
}

AddMarkUndoCommand::~AddMarkUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end addmarkundocommand" << std::endl;
    delete m_mark;
}

void AddMarkUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo addmarkundocommand" << std::endl;
    m_situation->addMark(m_mark);
}

void AddMarkUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo addmarkundocommand" << std::endl;
    m_situation->deleteMark(m_mark);
}

// Zone Mark
ZoneMarkUndoCommand::ZoneMarkUndoCommand(SituationModel *situation, const QList<MarkModel*> &markList, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_markList(markList) {
    if (debugLevel & 1 << COMMAND) std::cout << "new zonemarkundocommand" << std::endl;
}

ZoneMarkUndoCommand::~ZoneMarkUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end zonemarkundocommand" << std::endl;
}

void ZoneMarkUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo zonemarkundocommand" << std::endl;
    for(int i=0; i< m_markList.size(); i++) {
        MarkModel *mark = m_markList[i];
        mark->setZone(!mark->zone());
    }
}

void ZoneMarkUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo zonemarkundocommand" << std::endl;
    for(int i=0; i< m_markList.size(); i++) {
        MarkModel *mark = m_markList[i];
        mark->setZone(!mark->zone());
    }
}

bool ZoneMarkUndoCommand::mergeWith(const QUndoCommand *command) {
    const ZoneMarkUndoCommand *zoneCommand = static_cast<const ZoneMarkUndoCommand*>(command);
    if (m_markList != zoneCommand->m_markList)
        return false;
    undo();
    m_situation->undoStack()->setIndex(m_situation->undoStack()->index()-1);
    return true;
}

// Color Mark
ColorMarkUndoCommand::ColorMarkUndoCommand(SituationModel *situation, const QList<MarkModel*> &markList, const QColor &color, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_markList(markList),
        m_newColor(color) {
    if (debugLevel & 1 << COMMAND) std::cout << "new colormarkundocommand" << std::endl;
    foreach( MarkModel *mark, markList) {
        m_oldColors.append(mark->color());
    }
}

ColorMarkUndoCommand::~ColorMarkUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end colormarkundocommand" << std::endl;
}

void ColorMarkUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo colormarkundocommand" << std::endl;
    for(int i=0; i< m_markList.size(); i++) {
        MarkModel *mark = m_markList[i];
        mark->setColor(m_oldColors[i]);
    }
}

void ColorMarkUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo colormarkundocommand" << std::endl;
    for(int i=0; i< m_markList.size(); i++) {
        MarkModel *mark = m_markList[i];
        mark->setColor(m_newColor);
    }
}

bool ColorMarkUndoCommand::mergeWith(const QUndoCommand *command) {
    const ColorMarkUndoCommand *colorCommand = static_cast<const ColorMarkUndoCommand*>(command);
    if (m_markList != colorCommand->m_markList)
        return false;
    m_newColor = colorCommand->m_newColor;
    return true;
}

// Set Length
LengthMarkUndoCommand::LengthMarkUndoCommand(SituationModel* situation, const int length, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_oldLength(situation->situationLength()),
        m_newLength(length) {
    if (debugLevel & 1 << COMMAND) std::cout << "new lengthmarkundocommand" << std::endl;
}

LengthMarkUndoCommand::~LengthMarkUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end lengthmarkundocommand" << std::endl;
}

void LengthMarkUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo lengthmarkundocommand" << std::endl;
    m_situation->setSituationLength(m_oldLength);
}

void LengthMarkUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo lengthmarkundocommand" << std::endl;
    m_situation->setSituationLength(m_newLength);
}

bool LengthMarkUndoCommand::mergeWith(const QUndoCommand *command) {
    const LengthMarkUndoCommand *lengthmarkCommand = static_cast<const LengthMarkUndoCommand*>(command);
    if (m_situation != lengthmarkCommand->m_situation)
        return false;

    m_newLength = lengthmarkCommand->m_newLength;
    return true;
}

// Delete Mark
DeleteMarkUndoCommand::DeleteMarkUndoCommand(SituationModel* situation, MarkModel* mark, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_mark(mark) {
    if (debugLevel & 1 << COMMAND) std::cout << "new deletemarkundocommand" << std::endl;
}

DeleteMarkUndoCommand::~DeleteMarkUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end deletebmarkundocommand" << std::endl;
}

void DeleteMarkUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo deletebmarkundocommand" << std::endl;
    m_order = m_situation->deleteMark(m_mark);
}

void DeleteMarkUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo deletebmarkundocommand" << std::endl;
    m_situation->addMark(m_mark, m_order);
}

// Add PolyLine
AddPolyLineUndoCommand::AddPolyLineUndoCommand(SituationModel* situation, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation) {
    if (debugLevel & 1 << COMMAND) std::cout << "new addpolylineundocommand" << std::endl;
    m_polyLine = new PolyLineModel(situation);
}

AddPolyLineUndoCommand::~AddPolyLineUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end addpolylineundocommand" << std::endl;
    delete m_polyLine;
}

void AddPolyLineUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo addpolylineundocommand" << std::endl;
    m_situation->addPolyLine(m_polyLine);
}

void AddPolyLineUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo addpolylineundocommand" << std::endl;
    m_situation->deletePolyLine(m_polyLine);
}

// Delete PolyLine
DeletePolyLineUndoCommand::DeletePolyLineUndoCommand(SituationModel* situation, PolyLineModel* polyLine, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_situation(situation),
        m_polyLine(polyLine) {
    if (debugLevel & 1 << COMMAND) std::cout << "new removepolylineundocommand" << std::endl;
}

DeletePolyLineUndoCommand::~DeletePolyLineUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end removepolylineundocommand" << std::endl;
}

void DeletePolyLineUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo removepolylineundocommand" << std::endl;
    m_situation->deletePolyLine(m_polyLine);
}

void DeletePolyLineUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo removepolylineundocommand" << std::endl;
    m_situation->addPolyLine(m_polyLine);
}

// Add Point
AddPointUndoCommand::AddPointUndoCommand(PolyLineModel* polyLine, QPointF& position, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_polyLine(polyLine) {
    if (debugLevel & 1 << COMMAND) std::cout << "new addpointundocommand" << std::endl;
    m_point = new PointModel(polyLine);
    m_point->setPosition(position);
}

AddPointUndoCommand::~AddPointUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end addpointundocommand" << std::endl;
    delete m_point;
}

void AddPointUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo addpointundocommand" << std::endl;
    m_polyLine->addPoint(m_point);
}

void AddPointUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo addpointundocommand" << std::endl;
    m_polyLine->deletePoint(m_point);
}

// Delete Point
DeletePointUndoCommand::DeletePointUndoCommand(PolyLineModel* polyLine, PointModel* point, QUndoCommand *parent)
        : QUndoCommand(parent),
        m_polyLine(polyLine),
        m_point(point) {
    if (debugLevel & 1 << COMMAND) std::cout << "new deletepointundocommand" << std::endl;
}

DeletePointUndoCommand::~DeletePointUndoCommand() {
    if (debugLevel & 1 << COMMAND) std::cout << "end deletepointundocommand" << std::endl;
}

void DeletePointUndoCommand::redo() {
    if (debugLevel & 1 << COMMAND) std::cout << "redo deletepointundocommand" << std::endl;
    m_order = m_polyLine->deletePoint(m_point);
}

void DeletePointUndoCommand::undo() {
    if (debugLevel & 1 << COMMAND) std::cout << "undo deletepointundocommand" << std::endl;
    m_polyLine->addPoint(m_point, m_order);
}
