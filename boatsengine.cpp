
// C++ Implementation: boatsengine
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
#include "boatsengine.h"

#include "commontypes.h"
#include "boats.h"

#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"

#include "statemachine.h"
#include "scenarioanimation.h"
#include "xmlsituationreader.h"
#include "xmlsituationwriter.h"

#include <iostream>

extern int debugLevel;

BoatsEngine::BoatsEngine(QObject *parent) :
    QObject(parent),
    m_currentSituation(0)
{
}

void BoatsEngine::newFile() {
    SituationModel *situation = new SituationModel(this);
    m_situationList.append(situation);
    setIndex(m_situationList.size()-1);
    emit sizeChanged();
}

void BoatsEngine::setIndex(int index) {
    m_currentSituation = index;
    emit indexChanged();
}

void BoatsEngine::removeFile(int index) {
    SituationModel *situation = m_situationList.at(index);
    m_situationList.removeAt(index);
    situation->deleteLater();
    emit sizeChanged();
}

void BoatsEngine::resetFile() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->undoStack()->setIndex(0);
    setCurrentFile(situation, "");
    situation->undoStack()->clear();
    situation->wind().clearWind();
}

bool BoatsEngine::openFile(const QString &fileName) {
    SituationModel *situation = m_situationList.at(m_currentSituation);

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("Open Scenario File"),
//                             tr("Cannot read file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString()));
        return false;
    }

    XmlSituationReader reader(situation);
    if (!reader.read(&file)) {
//        QMessageBox::warning(this, tr("Open Scenario file"),
//                             tr("Parse error in file %1 at line %2, column %3:\n%4")
//                             .arg(fileName)
//                             .arg(reader.lineNumber())
//                             .arg(reader.columnNumber())
//                             .arg(reader.errorString()));
        return false;
    }
    setCurrentFile(situation, fileName);
    return true;
}

bool BoatsEngine::saveSituation(QString fileName) {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    QString name = fileName;
    if (name.isEmpty()) {
        QString defaultFile;
        if (situation->fileName().isEmpty()) {
            defaultFile = QDateTime::currentDateTime().toString("yyMMdd") + ".xbs";
        } else {
            defaultFile = situation->fileName();
        }
        if (QFileInfo(name).suffix().isEmpty())
            name.append(".xbs");
        if (QFile::exists(name)) {
//            if (QMessageBox::warning(this, tr("Save Scenario"),
//                                     tr("%1 already exists.\nDo you want to replace it?")
//                                     .arg(QFileInfo(name).baseName()),
//                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
//                    == QMessageBox::No) {
//                return false;
//            }
        }
    }

    QFile file(name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("Save Scenario"),
//                            tr("Cannot write file %1:\n%2.")
//                            .arg(fileName)
//                            .arg(file.errorString()));
        return false;
    }
    XmlSituationWriter writer(situation);
    writer.writeFile(&file);
    setCurrentFile(situation, name);
    return true;
}

void BoatsEngine::setCurrentFile(SituationModel *situation, const QString &fileName) {
    situation->setFileName(fileName);
    situation->undoStack()->setClean();
}

QStringList BoatsEngine::fileList() {
    QStringList fileList;
    foreach(SituationModel *situation, m_situationList) {
        if (!situation->fileName().isEmpty()) {
            fileList.append(situation->fileName());
        }
    }
    return fileList;
}

void BoatsEngine::addTrack() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->createTrack();
}

void BoatsEngine::deleteTrack() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->deleteTrack();
}

void BoatsEngine::addBoat() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->createBoat();
}

void BoatsEngine::deleteModels() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->deleteModels();
}

void BoatsEngine::addMark() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->createMark();
}

void BoatsEngine::addPolyLine() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->createLine();
}

void BoatsEngine::addPoint() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->createPoint();
}

void BoatsEngine::animate() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->animate();
}

void BoatsEngine::play() {
    if (debugLevel & 1 << ANIMATION) std::cout << "playing" << std::endl;
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->play();
}

void BoatsEngine::pause() {
    if (debugLevel & 1 << ANIMATION) std::cout << "pausing" << std::endl;
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->pause();
}

void BoatsEngine::stop() {
    if (debugLevel & 1 << ANIMATION) std::cout << "stopping" << std::endl;
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->stateMachine()->stop();
    situation->animation()->setCurrentTime(0);
}

void BoatsEngine::trimSail() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->trimSail();
}

void BoatsEngine::autotrimSail() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->autotrimSail();
}

void BoatsEngine::untrimSail() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->untrimSail();
}

void BoatsEngine::trimJib() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->trimJib();
}

void BoatsEngine::autotrimJib() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->autotrimJib();
}

void BoatsEngine::untrimJib() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->untrimJib();
}

void BoatsEngine::trimSpin() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->trimSpin();
}

void BoatsEngine::autotrimSpin() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->autotrimSpin();
}

void BoatsEngine::untrimSpin() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->untrimSpin();
}

void BoatsEngine::togglePortOverlap() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->togglePortOverlap();
}

void BoatsEngine::toggleStarboardOverlap() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleStarboardOverlap();
}

void BoatsEngine::toggleHidden() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleHidden();
}

void BoatsEngine::toggleText() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleText();
}

void BoatsEngine::toggleFlag(Boats::Flag flag) {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleFlag(flag);
}

void BoatsEngine::toggleAcceleration(Boats::Acceleration acceleration) {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleAcceleration(acceleration);
}

void BoatsEngine::toggleSpin() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleSpin();
}

void BoatsEngine::toggleMarkSide() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleMarkSide();
}

void BoatsEngine::toggleMarkArrow() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleMarkArrow();
}

void BoatsEngine::toggleMarkZone() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleMarkZone();
}

void BoatsEngine::setMarkColor(QColor color) {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->setMarkColor(color);
}

void BoatsEngine::toggleMarkLabel() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleMarkLabel();
}

void BoatsEngine::editMarkLabel(QString text) {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->editMarkLabel(text);
}

void BoatsEngine::toggleLaylines() {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->toggleLaylines();
}

void BoatsEngine::setLookAt(int direction, int tilt) {
    SituationModel *situation = m_situationList.at(m_currentSituation);
    situation->setLookAt(direction, tilt);
}
