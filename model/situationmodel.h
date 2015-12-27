//
// C++ Interface: situationmodel
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
#ifndef SITUATIONMODEL_H
#define SITUATIONMODEL_H

#include "boats.h"
#include "windmodel.h"

#include <QObject>
#include <QUndoStack>

class TrackModel;
class BoatModel;
class MarkModel;
class PolyLineModel;
class PointModel;
class StateMachine;
class ScenarioAnimation;


/**
    \class SituationModel

    \brief The main Model for a scenario

    This Class represents the Model for a Scenario, according to an
    Observer Pattern.

    It holds the actual data for the scenario and signals whoever is
    interested in changes to the model.

    There are mainly:
    - description attributes and technical data which define the scenario
    - a List of TrackModel, which in turn contains a List of BoatModel
    - a List of MarkModel

    \sa SituationModel, TrackModel, BoatModel, MarkModel
*/

class SituationModel : public QObject {
        Q_OBJECT
        Q_ENUMS(Boats::Series)
    public:

        Q_PROPERTY(QString title READ title WRITE changeTitle NOTIFY titleChanged)
        Q_PROPERTY(QString rules READ rules WRITE changeRules NOTIFY rulesChanged)
        Q_PROPERTY(bool grid READ showLayline WRITE toggleShowLayline NOTIFY showLaylineChanged)
        Q_PROPERTY(int laylineAngle READ laylineAngle WRITE changeLaylineAngle NOTIFY laylineChanged)
        Q_PROPERTY(Boats::Series situationSeries READ situationSeries WRITE changeSeries NOTIFY seriesChanged)
        Q_PROPERTY(int situationLength READ situationLength WRITE changeLength NOTIFY lengthChanged)
        Q_PROPERTY(QString abstract READ abstract WRITE changeAbstract NOTIFY abstractChanged)
        Q_PROPERTY(QString description READ description WRITE changeDescription NOTIFY descriptionChanged)
        Q_PROPERTY(qreal lookDirection READ lookDirection WRITE setLookDirection NOTIFY lookDirectionChanged)
        Q_PROPERTY(qreal tilt READ tilt WRITE setTilt NOTIFY tiltChanged)

        Q_PROPERTY(int size READ size NOTIFY tracksChanged)

        Q_PROPERTY(StateMachine* stateMachine READ stateMachine CONSTANT)
        Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
        Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)

        SituationModel(QObject *parent = 0);
        ~SituationModel();

        // Setters and Getters for Model Data
        QString title() const { return m_title; }
        void setTitle(const QString theValue);

        QString rules() const { return m_rules; }
        void setRules(const QString theValue);

        bool showLayline() const { return m_showLayline; }
        void setShowLayline(const bool theValue);

        int laylineAngle() const { return m_laylineAngle; }
        void setLaylineAngle(const int theValue);

        Boats::Series situationSeries() const { return m_situationSeries; }
        void setSituationSeries(const int theValue);

        int situationLength() const { return m_situationLength; }
        void setSituationLength(const int theValue);

        QString abstract() const { return m_abstract; }
        void setAbstract(const QString theValue);

        QString description() const { return m_description; }
        void setDescription(const QString theValue);

        qreal lookDirection() const { return m_lookDirection; }
        qreal tilt() const { return m_tilt; }

        int size() const { return m_tracks.size();}
        const QList<TrackModel*> tracks() const { return m_tracks; }

        int markSize() const { return m_marks.size();}
        const QList<MarkModel*> marks() const { return m_marks; }

        const QList<PolyLineModel*> polyLines() const { return m_lines; }

        WindModel& wind() { return m_wind; }

        // Setters and Getters for Non model Data
        QUndoStack * undoStack() const { return m_undoStack;}

        StateMachine *stateMachine(){ return m_stateMachine; }

        ScenarioAnimation *animation() const { return m_scenarioAnimation; }

        QList< PositionModel * > selectedModels() { return m_selectedModels; }
        QList< BoatModel * > selectedBoatModels() { return m_selectedBoatModels; }
        QList< MarkModel * > selectedMarkModels() { return m_selectedMarkModels; }
        QList< PointModel * > selectedPointModels() { return m_selectedPointModels; }

        QStringList discardedXml() const { return m_discardedXml; }
        void appendDiscardedXml(const QString& theValue);

        QString fileName() const { return m_fileName; }
        void setFileName(const QString theValue) {m_fileName = theValue; }

        // Helper to remotely trigger boat signals from elsewhere
        void addingBoat(BoatModel *boat) {emit boatAdded(boat);}
        void removingBoat(BoatModel *boat) {emit boatRemoved(boat);}

        // Helper to remotely trigger point signals from elsewhere
        void addingPoint(PointModel *point) {emit pointAdded(point);}
        void removingPoint(PointModel *point) {emit pointRemoved(point);}

        // UndoCommand actions
        Q_INVOKABLE void undo() { m_undoStack->undo(); }
        bool canUndo() { return m_undoStack->canUndo(); }
        Q_INVOKABLE void redo() { m_undoStack->redo(); }
        bool canRedo() { return m_undoStack->canRedo(); }

        Q_INVOKABLE void changeTitle(QString title);
        Q_INVOKABLE void changeRules(QString rules);
        Q_INVOKABLE void toggleShowLayline(bool showlayline);
        Q_INVOKABLE void changeSeries(Boats::Series series);
        Q_INVOKABLE void changeLaylineAngle(int angle);
        Q_INVOKABLE void changeLength(int length);
        Q_INVOKABLE void changeAbstract(QString abstract);
        Q_INVOKABLE void changeDescription(QString description);
        Q_INVOKABLE void toggleWind();

        Q_INVOKABLE void rotateModel(qreal angle);
        Q_INVOKABLE void deleteModels();
        Q_INVOKABLE void deleteTrack();

        Q_INVOKABLE void setColor(QColor color);
        Q_INVOKABLE void setShowPath();
        Q_INVOKABLE void setSeries(Boats::Series series);
        Q_INVOKABLE void setFollowTrack();
        Q_INVOKABLE void addWind(qreal wind);
        Q_INVOKABLE void setWind(int index, qreal wind);
        Q_INVOKABLE void deleteWind(int index);
        Q_INVOKABLE void trimSail();
        Q_INVOKABLE void autotrimSail();
        Q_INVOKABLE void untrimSail();
        Q_INVOKABLE void trimJib();
        Q_INVOKABLE void autotrimJib();
        Q_INVOKABLE void untrimJib();
        Q_INVOKABLE void trimSpin();
        Q_INVOKABLE void autotrimSpin();
        Q_INVOKABLE void untrimSpin();
        Q_INVOKABLE void togglePortOverlap();
        Q_INVOKABLE void toggleStarboardOverlap();
        Q_INVOKABLE void toggleFlag(Boats::Flag flag);
        Q_INVOKABLE void toggleAcceleration(Boats::Acceleration acceleration);
        Q_INVOKABLE void toggleHidden();
        Q_INVOKABLE void toggleText();
        Q_INVOKABLE void setText(QString text);
        Q_INVOKABLE void moveText(QPointF pos);
        Q_INVOKABLE void toggleSpin();
        Q_INVOKABLE void toggleMarkSide();
        Q_INVOKABLE void toggleMarkArrow();
        Q_INVOKABLE void toggleMarkZone();
        Q_INVOKABLE void setMarkColor(QColor color);
        Q_INVOKABLE void toggleMarkLabel();
        Q_INVOKABLE void editMarkLabel(QString text);
        Q_INVOKABLE void toggleLaylines();
        Q_INVOKABLE void setLookAt(int direction, int tilt);

        // Tracks
        void addTrack(TrackModel *track, int order = -1);
        void deleteTrack(TrackModel *track);

        // Marks
        void addMark(MarkModel *mark, int order = -1);
        int deleteMark(MarkModel *mark);

        // Lines
        void addPolyLine(PolyLineModel *polyline, int order = -1);
        void deletePolyLine(PolyLineModel *polyline);

        Q_INVOKABLE void setCurPosition(QPointF pos);
        QPointF curPosition() { return m_curPosition; }

        // selection mechanism
        Q_INVOKABLE void clearSelectedModels();
        Q_INVOKABLE void addSelectedBoat(BoatModel *boat);
        Q_INVOKABLE void addSelectedMark(MarkModel *mark);
        Q_INVOKABLE void addSelectedPoint(PointModel *point);
        Q_INVOKABLE void addSelectedModel(PositionModel *position);

        void setLookDirection(qreal theValue);
        void setTilt(qreal theValue);

    public slots:
        // Wind
        void resetWind();

        // Slots for state signals
        void createTrack();
        void createBoat();
        void createMark();
        void createLine();
        void createPoint();

        void moveModel();
        void rotateModel();
        void exitCreateState();

    signals:
        // Signals for Track
        void trackAdded(TrackModel *track);
        void trackRemoved(TrackModel *track);
        void tracksChanged();

        // Signals for Boat
        void boatAdded(BoatModel *boat);
        void boatRemoved(BoatModel *boat);

        // Signals for Scenario Parameters
        void titleChanged(const QString title);
        void rulesChanged(const QString rules);
        void showLaylineChanged(const bool show);
        void laylineChanged(const int angle);
        void seriesChanged(const int series);
        void lengthChanged(const int length);
        void abstractChanged(const QString abstract);
        void descriptionChanged(const QString description);
        void lookDirectionChanged( const qreal lookDirection);
        void tiltChanged( const qreal tilt);

        // Signals for Marks
        void markAdded(MarkModel *mark);
        void markRemoved(MarkModel *mark);

        // Signals for Lines
        void polyLineAdded(PolyLineModel *polyline);
        void polyLineRemoved(PolyLineModel *polyline);

        // Signals for Line Points
        void pointAdded(PointModel *point);
        void pointRemoved(PointModel *point);

        void canUndoChanged(bool canUndo);
        void canRedoChanged(bool canRedo);

    private:
        // Model Data
        /// \a m_title holds the Title of the Scenario
        QString m_title;

        /// \a m_rules holds the Rules of the Scenario
        QString m_rules;

        /// \a m_abstract holds the Abstract of the Scenario
        QString m_abstract;

        /// \a m_description holds the Description of the Scenario
        QString m_description;

        /// \a m_showLayline holds whether the Laylines will be displayed
        bool m_showLayline;

        /// \a m_laylineAngle holds the Layline Angle of the Scenario
        int m_laylineAngle;

        /// \a m_situationSeries holds the Main Series of the Scenario
        Boats::Series m_situationSeries;

        /// \a m_situationLength holds the size of the Zone at Marks of
        /// the Scenario
        int m_situationLength;

        /// \a m_lookDirection holds the direction of the view
        qreal m_lookDirection;

        /// \a m_tilt holds the tilt of the view
        qreal m_tilt;

        /// \a m_wind holds the WindModel of the Scenario
        WindModel m_wind;

        /// \a m_tracks holds the List of Tracks of the Scenario
        QList<TrackModel*> m_tracks;

        /// \a m_marks holds the List of Marks of the Scenario
        QList<MarkModel*> m_marks;

        /// \a m_lines holds the List of PolyLines of the Scenario
        QList<PolyLineModel*> m_lines;

        // Non model Data
        /// \a m_undoStack maintains the Undo Stack for the Scenario
        QUndoStack *m_undoStack;

        /// \a m_state holds the SceneState for the current scenario
        StateMachine *m_stateMachine;

        /// \a m_scenarioAnimation holds the general AnimationGroup
        /// manipulated during animation mode
        ScenarioAnimation* m_scenarioAnimation;

        /// \a m_curPosition holds the QPointF where mouse was last seen
        QPointF m_curPosition;

        // Bookkeeping references to selected models
        /// \a m_selectedModels holds the list of selected PositionModel
        QList<PositionModel*> m_selectedModels;

        /// \a m_selectedBoatsModels holds the list of selected BoatModel
        QList<BoatModel*> m_selectedBoatModels;

        /// \a m_selectedMarkModels holds the list of selected MarkModel
        QList<MarkModel*> m_selectedMarkModels;

        /// \a m_selectedPointModels holds the list of selected PointModel
        QList<PointModel*> m_selectedPointModels;

        /// \a m_discardedXml keeps all unparsed xml tags
        QStringList m_discardedXml;

        /// \a m_fileName holds the name of the file on disk
        QString m_fileName;
};

#endif
