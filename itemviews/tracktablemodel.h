//
// C++ Interface: tracktablemodel
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
#ifndef TRACKTABLEMODEL_H
#define TRACKTABLEMODEL_H

#include <QtGui>

#include "commontypes.h"

#include "situationmodel.h"
class TrackModel;

class TrackTableModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        TrackTableModel(SituationModel* situation = 0, QObject *parent = 0);
        ~TrackTableModel();

        void setSituation(SituationModel *situation);
        int rowCount(const QModelIndex &parent) const { Q_UNUSED(parent); return m_situation->size(); }
        int columnCount(const QModelIndex &parent) const { Q_UNUSED(parent); return 4;}
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

    public slots:
        void addTrack(TrackModel *track);
        void updateTrack();
        void deleteTrack(TrackModel *track);

    private:
        SituationModel *m_situation;
};

#endif
