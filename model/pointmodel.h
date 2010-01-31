//
// C++ Interface: pointmodel
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2009-2010 Thibaut GRIDEL
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
#ifndef POINTMODEL_H
#define POINTMODEL_H

#include <QtGui>

#include "positionmodel.h"

class PolyLineModel;

/**
    \class PointModel

    \brief The Model for a Point Position

    The class represents  the Model for a Point, according to an
    Observer Pattern.

    PointModel inherits PositionModel and contains data which represents
    one point at a given time, as part of a PolyLine.

    \sa SituationModel, PolyLineModel

*/

class PointModel : public PositionModel {
        Q_OBJECT
    public:
        PointModel(PolyLineModel *polyline, QObject *parent = 0);
        ~PointModel();

        // Setters and Getters for Model Data
        virtual void setPosition(const QPointF& theValue);

        // Setters and Getters for Non model Data
        PolyLineModel* polyLine() const { return m_polyline; }

    private:
        // Non model Data
        /// \a m_polyline keeps a pointer to the PolyLineModel to which
        /// it belongs
        PolyLineModel *m_polyline;
};

#endif
