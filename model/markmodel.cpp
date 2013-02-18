//
// C++ Implementation: markmodel
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2010 Thibaut GRIDEL
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

#include "markmodel.h"

#include "commontypes.h"
#include "situationmodel.h"

extern int debugLevel;

MarkModel::MarkModel(SituationModel* situation, QObject *parent)
        : PositionModel(situation, parent),
        m_color(Qt::gray),
        m_zone(false),
        m_arrowVisible(true),
        m_leaveToPort(true),
        m_labelVisible(true),
        m_length(situation->situationLength()) {
    if (debugLevel & 1 << MODEL) std::cout << "new Mark " << this << std::endl;
    setOrder(situation->markSize()+1);
    setLabelText(QString::number(order()));
}

MarkModel::~MarkModel() {
    if (debugLevel & 1 << MODEL) std::cout << "delete Mark " << this << std::endl;
}

void MarkModel::setColor(const QColor& theValue) {
    if (theValue != m_color) {
        m_color = theValue;
        emit colorChanged(m_color);
    }
}

void MarkModel::setZone(const bool theValue) {
    if (theValue != m_zone) {
        if (debugLevel & 1 << MODEL) std::cout << "Mark " << this
        << " zone " << theValue << std::endl;
        m_zone = theValue;
        emit zoneChanged(m_zone);
    }
}

void MarkModel::setLength(const int theValue) {
    if (theValue != m_length) {
        if (debugLevel & 1 << MODEL) std::cout << "Mark " << this
        << " length " << theValue << std::endl;
        m_length = theValue;
        emit lengthChanged(m_length);
    }
}

void MarkModel::setArrowVisible(const bool theValue) {
    if (theValue != m_arrowVisible) {
        if (debugLevel & 1 << MODEL) std::cout << "Mark " << this
        << " arrowVisible " << theValue << std::endl;
        m_arrowVisible = theValue;
        emit arrowVisibilityChanged(m_arrowVisible);
    }
}

void MarkModel::setLeaveToPort(const bool theValue) {
    if (theValue != m_leaveToPort) {
        if (debugLevel & 1 << MODEL) std::cout << "Mark " << this
        << " leaveToPort " << theValue << std::endl;
        m_leaveToPort = theValue;
        emit leaveToPortChanged(m_leaveToPort);
    }
}

void MarkModel::setLabelVisible(const bool theValue) {
    if (theValue != m_labelVisible) {
        if (debugLevel & 1 << MODEL) std::cout << "Mark " << this
        << " labelVisible " << theValue << std::endl;
        m_labelVisible = theValue;
        emit labelVisibilityChanged(m_labelVisible);
    }
}

void MarkModel::setLabelText(const QString theValue) {
    if (theValue != m_labelText) {
        if (debugLevel & 1 << MODEL) std::cout << "Mark " << this
        << " labelText " << theValue.toStdString() << std::endl;
        m_labelText = theValue;
        emit labelTextChanged(m_labelText);
    }
}
