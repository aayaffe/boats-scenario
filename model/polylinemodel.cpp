//
// C++ Implementation: polylinemodel
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

#include "polylinemodel.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "pointmodel.h"

extern int debugLevel;

PolyLineModel::PolyLineModel(SituationModel* situation, QObject *parent)
        : QObject(parent),
        m_situation(situation) {
    if (debugLevel & 1 << MODEL) std::cout << "new PolyLine " << this << std::endl;
}

PolyLineModel::~PolyLineModel() {
    if (debugLevel & 1 << MODEL) std::cout << "delete PolyLine " << this << std::endl;
}

void PolyLineModel::appendDiscardedXml(const QString& theValue) {
    if (!m_discardedXml.contains(theValue)) {
        m_discardedXml.append(theValue);
    }
}

void PolyLineModel::changingPolyLine(PolyLineModel *polyline) {
    QPainterPath path;
    if (m_points.size() < 1) {
        m_path = path;
        return;
    }

    path.moveTo(m_points[0]->position());
    foreach(PointModel *point, m_points) {
        path.lineTo(point->position());
    }
    m_path = path;
    emit polyLineChanged(polyline);
}

PointModel* PolyLineModel::addPoint(PointModel *point, int order) {
    if (order == -1) {
        order = m_points.size();
    }
    m_points.insert(order, point);
    m_situation->addingPoint(point);
    if (debugLevel & 1 << MODEL) std::cout << "Adding Point " << order+1 << std::endl;
    changingPolyLine(this);
    return point;
}

int PolyLineModel::deletePoint(PointModel *point) {
    int order = m_points.indexOf(point);
    m_points.removeOne(point);
    if (debugLevel & 1 << MODEL) std::cout << "Removing Point " << order+1 << std::endl;
    m_situation->removingPoint(point);
    changingPolyLine(this);
    return order;
}

void PolyLineModel::displayPoints() {
    if (debugLevel & 1 << MODEL) std::cout << "Displaying points" << std::endl;
    foreach (PointModel* point, m_points) {
        m_situation->addingPoint(point);
    }
}

void PolyLineModel::hidePoints() {
    if (debugLevel & 1 << MODEL) std::cout << "Hiding points" << std::endl;
    foreach (PointModel* point, m_points) {
        m_situation->removingPoint(point);
    }
}
