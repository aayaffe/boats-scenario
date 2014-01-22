//
// C++ Interface: boatsengine
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2014 Thibaut GRIDEL
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
#ifndef BOATSENGINE_H
#define BOATSENGINE_H

#include "boat.h"

#include <QObject>
#include <QStringList>
#include <QTranslator>

class SituationModel;

class BoatsEngine : public QObject
{
    Q_OBJECT
public:
    explicit BoatsEngine(QObject *parent = 0);
    
    SituationModel *currentModel() { return m_situationList.at(m_currentSituation); }
    int situationSize() { return m_situationList.size(); }
    int currentIndex() { return m_currentSituation; }
    QStringList fileList();

    bool saveSituation(SituationModel *situation, QString name);

public slots:

    // File actions
    void newFile();
    void setIndex(int index);
    void removeFile(int index);
    void resetFile();
    bool openFile(const QString &fileName);

    // State actions
    void addTrack();
    void addBoat();
    void addMark();
    void addPolyLine();
    void addPoint();

    // Boat actions
    void trimSail();
    void autotrimSail();
    void untrimSail();
    void togglePortOverlap();
    void toggleStarboardOverlap();
    void toggleFlag(Boats::Flag flag);
    void toggleAcceleration(Boats::Acceleration acceleration);
    void toggleHidden();
    void toggleText();
    void toggleSpin();

    // Mark actions
    void toggleMarkSide();
    void toggleMarkArrow();
    void toggleMarkZone();
    void setMarkColor(QColor color);
    void toggleLaylines();
    void toggleMarkLabel();
    void editMarkLabel(QString text);

    // Situation actions
    void deleteTrack();
    void deleteModels();
    void setLookAt(int direction, int tilt);

private:

    // File methods
    void setCurrentFile(SituationModel *situation, const QString &fileName);

    // GraphicsView Framework
    QList<SituationModel *> m_situationList;
    int m_currentSituation;
};

#endif
