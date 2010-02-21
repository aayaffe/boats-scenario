//
// C++ Interface: situationmodel
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2009 Thibaut GRIDEL
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

#include <QtGui>

#include "boats.h"

class TrackModel;
class BoatModel;
class MarkModel;
class PolyLineModel;
class PointModel;

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
    public:
        SituationModel(QObject *parent = 0);
        ~SituationModel();

        int sizeForSeries(const Boats::Series series);

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

        int size() const { return m_tracks.size();}
        const QList<TrackModel*> tracks() const { return m_tracks; }

        int markSize() const { return m_marks.size();}
        const QList<MarkModel*> marks() const { return m_marks; }

        const QList<PolyLineModel*> polyLines() const { return m_lines; }

        // Setters and Getters for Non model Data
        QUndoStack * undoStack() const { return m_undoStack;}

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

    signals:
        // Signals for Track
        void trackAdded(TrackModel *track);
        void trackRemoved(TrackModel *track);

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

        // Signals for Marks
        void markAdded(MarkModel *mark);
        void markRemoved(MarkModel *mark);

        // Signals for Lines
        void polyLineAdded(PolyLineModel *polyline);
        void polyLineRemoved(PolyLineModel *polyline);

        // Signals for Line Points
        void pointAdded(PointModel *point);
        void pointRemoved(PointModel *point);

    public slots:
        // Slots for Tracks
        void addTrack(TrackModel *track, int order = -1);
        void deleteTrack(TrackModel *track);

        // Slots for Marks
        void addMark(MarkModel *mark, int order = -1);
        int deleteMark(MarkModel *mark);

        // Slots for Lines
        void addPolyLine(PolyLineModel *polyline, int order = -1);
        void deletePolyLine(PolyLineModel *polyline);

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

        /// \a m_tracks holds the List of Tracks of the Scenario
        QList<TrackModel*> m_tracks;

        /// \a m_marks holds the List of Marks of the Scenario
        QList<MarkModel*> m_marks;

        /// \a m_lines holds the List of PolyLines of the Scenario
        QList<PolyLineModel*> m_lines;


        // Non model Data
        /// \a m_undoStack maintains the Undo Stack for the Scenario
        QUndoStack *m_undoStack;

        /// \a m_discardedXml keeps all unparsed xml tags
        QStringList m_discardedXml;

        /// \a m_fileName holds the name of the file on disk
        QString m_fileName;
};

#endif
