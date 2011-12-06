//
// C++ Interface: windmodel
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
#ifndef WINDMODEL_H
#define WINDMODEL_H

#include "positionmodel.h"

/**
    \class PointModel

    \brief The Model for Wind directions

    The class represents  the Model for Wind, according to an
    Observer Pattern.

    WindModel inherits PositionModel and contains data which represents
    the directions of the wind of the scenario

    \sa SituationModel

*/

class WindModel : public PositionModel {
        Q_OBJECT

        Q_PROPERTY(qreal direction READ direction WRITE setDirection)

    public:
        WindModel(SituationModel* situation, QObject *parent = 0);
        ~WindModel();

        // Setters and Getters for Model Data
        void addWind(const qreal direction, int order = -1);
        void deleteWind(int order = -1);
        void clearWind();

        int size() const { return m_directions.size(); };
        qreal windAt(int order = -1) const;
        void setWindAt(qreal direction, int order);

        // Setters and Getters for Non model Data
        qreal direction() const { return m_direction; }
        void setDirection(qreal direction);

    signals:
        void windReset();
        void directionChanged(qreal);

    private:
        // Model Data
        QList<qreal> m_directions;

        // Non model Data
        /// m_direction holds the current wind, signalling animation
        qreal m_direction;
};

#endif // WINDMODEL_H
