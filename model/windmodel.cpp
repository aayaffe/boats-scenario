//
// C++ Implementation: windmodel
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

#include "windmodel.h"
#include "commontypes.h"

extern int debugLevel;

WindModel::WindModel(SituationModel *situation, QObject *parent)
    : PositionModel(situation, parent),
    m_visible(false) {
    if (debugLevel & 1 << MODEL) std::cout << "new Wind " << this << std::endl;
}

WindModel::~WindModel() {
    if (debugLevel & 1 << MODEL) std::cout << "delete Wind " << this << std::endl;
}

void WindModel::setVisible(bool visible) {
    if (m_visible != visible) {
        m_visible = visible;
        emit windVisibleChanged(visible);
    }
}

void WindModel::addWind(const qreal direction, int order) {
    if (order == -1) {
        order = m_directions.size();
    }
    m_directions.insert(order, direction);
    emit windReset();
    if (debugLevel & 1 << MODEL) std::cout << "Adding Wind " << order+1 << std::endl;
}

void WindModel::deleteWind(int order) {
    m_directions.removeAt(order);
    emit windReset();
    if (debugLevel & 1 << MODEL) std::cout << "Removing Wind " << order+1 << std::endl;
}

void WindModel::clearWind() {
    setVisible(false);
    setPosition(QPointF());
    m_discardedXml.clear();
    m_directions.clear();
    emit windReset();
}

qreal WindModel::windAt(int order) const {
    if (order == -1 || order >= m_directions.size()) {
        order = m_directions.size()-1;
    } else if (order < -1) {
        order = 0;
    }
    if (m_directions.isEmpty()) {
        return 0;
    }
    return m_directions.at(order);
}

void WindModel::setWindAt(qreal direction, int order) {
    if (order < 0 || order > m_directions.size()) {
        return;
    }
    m_directions[order] = direction;
    if (debugLevel & 1 << MODEL) std::cout << "wind for " << order+1 << " " << direction;
    emit windReset();
}

void WindModel::setDirection(qreal direction) {
    m_direction = direction;
    emit directionChanged(m_direction);
}
