//
// C++ Interface: situationmodel
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SITUATIONMODEL_H
#define SITUATIONMODEL_H

#include <QtGui>

#include "commontypes.h"

class TrackModel;
class BoatModel;
class MarkModel;

class SituationModel : public QObject {
        Q_OBJECT
    public:
        SituationModel(QObject *parent = 0);
        ~SituationModel();

        QUndoStack * undoStack() const { return m_undoStack;};
        QList<QString> seriesNames() {return m_seriesNames; };

        Series situationSeries() const { return m_situationSeries; };

        int size() const { return m_tracks.size();};
        const QList<TrackModel*> tracks() const { return m_tracks; };

        int markSize() const { return m_marks.size();};
        const QList<MarkModel*> marks() const { return m_marks; };

        qreal laylineAngle() const { return m_laylineAngle; };
        void setLaylineAngle(const int theValue, bool update = false);

        QStringList discardedXml() const { return m_discardedXml; };
        void appendDiscardedXml(const QString& theValue);

        QString fileName() const { return m_fileName; };
        void setFileName(const QString theValue) {m_fileName = theValue; };

        void addingBoat(BoatModel *boat) {emit boatAdded(boat);};
        void removingBoat(BoatModel *boat) {emit boatRemoved(boat);};

    signals:
        void trackAdded(TrackModel *track);
        void trackRemoved(TrackModel *track);
        void boatAdded(BoatModel *boat);
        void boatRemoved(BoatModel *boat);
        void laylineChanged(const int angle);
        void markAdded(MarkModel *mark);
        void markRemoved(MarkModel *mark);

    public slots:
        void setSeries(const int theValue);
        void addTrack(TrackModel *track);
        void deleteTrack(TrackModel *track);
        void addMark(MarkModel *mark, int order = 0);
        int deleteMark(MarkModel *mark);

    private:
        QUndoStack *m_undoStack;
        QList<QString> m_seriesNames;
        QString m_fileName;
        int m_laylineAngle;
        Series m_situationSeries;
        QList<TrackModel*> m_tracks;
        QList<MarkModel*> m_marks;
        QStringList m_discardedXml;

};

#endif
