//
// C++ Implementation: windtablemodel
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
#include <iostream>

#include "windtablemodel.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "windmodel.h"
#include "undocommands.h"

extern int debugLevel;

WindTableModel::WindTableModel(WindModel *wind, QObject *parent)
        : QAbstractTableModel(parent),
        m_wind(wind) {
    if (debugLevel & 1 << MODEL) std::cout << "new windtable " << this << std::endl;
}

WindTableModel::~WindTableModel() {
    if (debugLevel & 1 << MODEL) std::cout << "end windtable " << this << std::endl;
}

void WindTableModel::setWind(WindModel *wind)  {
    if (m_wind != wind) {
        beginResetModel();
        m_wind = wind;
        endResetModel();
    }
}

QVariant WindTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_wind->size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case WIND_INDEX:
                return index.row()+1;
                break;
            case WIND_DIRECTION:
                return m_wind->windAt(index.row());
                break;
            default:
                return QVariant();
                break;
        }
    } else if (role == Qt::CheckStateRole) {
        switch (index.column()) {
            case WIND_DELETE:
                return false;
                break;
            default:
                return QVariant();
                break;
        }
    }
    return QVariant();
}

QVariant WindTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case WIND_INDEX:
                return tr("Index");

            case WIND_DIRECTION:
                return tr("Direction");

            case WIND_DELETE:
                return tr("Delete");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags WindTableModel::flags(const QModelIndex &index) const {
    Q_UNUSED(index);
    return Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsUserCheckable;
}

bool WindTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    Q_UNUSED(role);
    if (!index.isValid())
        return false;

    if (index.row() > m_wind->size() || index.row() < 0)
        return false;

    switch (index.column()) {
        case WIND_DIRECTION:
            if (value.canConvert<qreal>()) {
                qreal newValue = value.value<qreal>();
                if (debugLevel & 1 << MODEL) std::cout << "setting wind " << newValue;
                if (index.row() == m_wind->size()) {
                    m_wind->situation()->undoStack()->push(new AddWindUndoCommand(m_wind, newValue));
                } else {
                    if (newValue != m_wind->windAt(index.row())) {
                        m_wind->situation()->undoStack()->push(new SetWindUndoCommand(m_wind, index.row(), newValue));
                    }
                }
                return true;
            }
            break;
        case WIND_DELETE:
            if (debugLevel & 1 << MODEL) std::cout << "deleting index " << index.row();
            if (index.row() < m_wind->size()) {
                m_wind->situation()->undoStack()->push(new DeleteWindUndoCommand(m_wind, index.row()));
            }
            return true;
            break;
        default:
            return false;
            break;
    }
    return false;
}

void WindTableModel::updateWind() {
    beginResetModel();
    endResetModel();
}
