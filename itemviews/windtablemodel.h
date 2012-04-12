//
// C++ Interface: windtablemodel
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2011 Thibaut GRIDEL
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
#ifndef WINDTABLEMODEL_H
#define WINDTABLEMODEL_H

#include <QtGui>

#include "commontypes.h"

#include "windmodel.h"

class WindTableModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        WindTableModel(WindModel* wind = 0, QObject *parent = 0);
        ~WindTableModel();

        void setWind(WindModel *wind);
        int rowCount(const QModelIndex &parent) const { Q_UNUSED(parent); return m_wind->size()+1; }
        int columnCount(const QModelIndex &parent) const { Q_UNUSED(parent); return 3;}
        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

    public slots:
        void updateWind();

    private:
        WindModel *m_wind;
};

#endif
