//
// C++ Interface: trackmodel
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QtGui>

#include "boats.h"

class SituationModel;
class BoatModel;

class TrackModel : public QObject {
        Q_OBJECT
    public:
        TrackModel(SituationModel* situation = 0, QObject *parent = 0);
        ~TrackModel();

        BoatModel * addBoat(BoatModel *boat, int order = 0);
        int deleteBoat(BoatModel *boat);

        void displayBoats();
        void hideBoats();

        SituationModel* situation() const { return m_situation; }
        QColor color() const { return m_color;};
        void setColor(const QColor& theValue, bool update = false);
        Boats::Series series() const { return m_series;};
        void setSeries(const Boats::Series theValue, bool update = false);
        int length() const { return m_length; };
        int size() const { return m_boats.size();};
        const QList<BoatModel*> boats() const { return m_boats; };
        const QPainterPath path() const { return m_path; };

        QStringList discardedXml() const { return m_discardedXml; };
        void appendDiscardedXml(const QString& theValue);

        void changingTrack(TrackModel *track);

    signals:
        void colorChanged(QColor color);
        void seriesChanged(Boats::Series series);
        void trackChanged(TrackModel *track);

    private:
        SituationModel *m_situation;
        QColor m_color;
        Boats::Series m_series;
        int m_length;
        QList<BoatModel*> m_boats;
        QPainterPath m_path;
        QStringList m_discardedXml;
};

#endif
