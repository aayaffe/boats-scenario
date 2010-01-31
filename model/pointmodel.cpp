//
// C++ Implementation: pointmodel
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
#include <iostream>
#include <cmath>

#include "pointmodel.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "polylinemodel.h"

extern int debugLevel;

PointModel::PointModel(PolyLineModel* polyline, QObject *parent)
        : PositionModel(parent),
        m_polyline(polyline) {
    if (debugLevel & 1 << MODEL) std::cout << "new Point " << this << std::endl;
    setOrder(polyline->size()+1);
}

PointModel::~PointModel() {
    if (debugLevel & 1 << MODEL) std::cout << "delete Point " << this << std::endl;
}

void PointModel::setPosition(const QPointF& theValue) {
    PositionModel::setPosition(theValue);
    m_polyline->changingPolyLine(m_polyline);
}
