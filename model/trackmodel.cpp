//
// C++ Implementation: trackmodel
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2011 Thibaut GRIDEL
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

#include "trackmodel.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "boatmodel.h"

extern int debugLevel;

TrackModel::TrackModel(SituationModel *situation, QObject *parent)
        : QObject(parent),
        m_color(),
        m_series(Boats::unknown),
        m_showPath(true),
        m_followTrack(false),
        m_situation(situation),
        m_length(0) {
    m_order = situation->size();
    if (debugLevel & 1 << MODEL) std::cout << "new track " << this << std::endl;
    switch (situation->size() % 6) {
        case 0: m_color = QColor(Qt::yellow); break;
        case 1: m_color = QColor(Qt::blue); break;
        case 2: m_color = QColor(Qt::green); break;
        case 3: m_color = QColor(Qt::red); break;
        case 4: m_color = QColor(Qt::cyan); break;
        case 5: m_color = QColor(Qt::magenta); break;
    }
    emit colorChanged(m_color);
    setSeries(situation->situationSeries());
}

TrackModel::~TrackModel() {
    if (debugLevel & 1 << MODEL) std::cout << "end track " << this << std::endl;
}

void TrackModel::setOrder(const int theValue) {
    if (theValue != m_order) {
        m_order = theValue;
        emit orderChanged(m_order);
    }
}

void TrackModel::setColor(const QColor& theValue) {
    if (theValue != m_color) {
        m_color = theValue;
        emit colorChanged(m_color);
    }
}

void TrackModel::setShowPath(const bool theValue) {
    if (theValue != m_showPath) {
        m_showPath = theValue;
        emit showPathChanged(m_showPath);
    }
}

void TrackModel::setFollowTrack(bool theValue) {
    if (theValue != m_followTrack) {
        m_followTrack = theValue;
        emit followTrackChanged(m_followTrack);
    }
    if (m_followTrack && m_situation) {
        foreach (TrackModel *model, m_situation->tracks()) {
            if (model != this) {
                model->setFollowTrack(false);
            }
        }
    }
}

void TrackModel::setSeries(const Boats::Series theValue) {
    if (theValue != m_series) {
        m_series = theValue;
        m_length = Boats::seriesSizeList()[m_series];
        emit seriesChanged(m_series);
    }
}

void TrackModel::appendDiscardedXml(const QString& theValue) {
    if (!m_discardedXml.contains(theValue)) {
        m_discardedXml.append(theValue);
    }
}


BoatModel * TrackModel::addBoat(BoatModel *boat, int order) {
    if (order == -1) {
        order = m_boats.size();
    }
    m_boats.insert(order, boat);
    if (debugLevel & 1 << MODEL) std::cout << "Adding Boat " << order+1 << std::endl;
    for (int i=order+1; i<m_boats.size(); i++) {
        m_boats[i]->setOrder(i+1);
    }
    m_situation->addingBoat(boat);
    changingTrack(this);
    emit boatsChanged();
    return boat;
}

int TrackModel::deleteBoat(BoatModel *boat) {
    int order = m_boats.indexOf(boat);
    m_boats.removeOne(boat);
    if (debugLevel & 1 << MODEL) std::cout << "Removing Boat " << order+1 << std::endl;
    for (int i=order; i<m_boats.size(); i++) {
        m_boats[i]->setOrder(i+1);
    }
    m_situation->removingBoat(boat);
    changingTrack(this);
    emit boatsChanged();
    return order;
}

void TrackModel::displayBoats() {
    if (debugLevel & 1 << MODEL) std::cout << "Displaying boats" << std::endl;
    foreach (BoatModel* boat, m_boats) {
        m_situation->addingBoat(boat);
    }
}

void TrackModel::hideBoats() {
    if (debugLevel & 1 << MODEL) std::cout << "Hiding boats" << std::endl;
    foreach (BoatModel* boat, m_boats) {
        m_situation->removingBoat(boat);
    }
}

// calculate new heading:
// from position of head of index boat to new potential position
qreal TrackModel::headingForNext(int index, QPointF point) {
    const BoatModel* boat = m_boats.at(index);
    qreal length = m_length / 2.0;
    qreal theta0 = boat->heading() * M_PI /180;
    QPointF point2 = point - (boat->position() + QPointF(length*sin(theta0),-length*cos(theta0)));
    return fmod(atan2 (point2.x(), -point2.y()) * 180 / M_PI + 360.0, 360.0);
}

void TrackModel::changingTrack(TrackModel *track) {
    QPainterPath path;
    QPainterPath wholePath;

    if (m_boats.size() < 1) {
        m_path = path;
        return;
    }

    QPointF pos0(m_boats[0]->position());
    qreal heading0 = m_boats[0]->heading();

    wholePath.moveTo(pos0);
    BoatModel *oldBoat = m_boats[0];
    int i = 1;
    while (i < m_boats.size() && m_boats[i]->dim() ) {
        path.moveTo(pos0);
        BoatModel *boat = m_boats[i];
        QPointF pos1(boat->position());
        qreal heading1 = boat->heading();

        // distance and angle between positions
        QPointF delta = pos1-pos0;
        qreal dist = sqrt(pow(delta.x(),2) + pow(delta.y(),2));
        qreal theta = fmod(atan2 (delta.x(), -delta.y()) *180/M_PI +360, 360);

        // empirical factor for control point distance
        qreal factor = dist*2/5;

        QPointF c1(pos0);
        // stalled condition when next boat in the back
        qreal angle0 = fmod(theta - heading0 +360, 360);
        bool stalled0 = ((angle0 >= 90) && (angle0 <= 270));
        if (!stalled0) {
            c1 += QPointF(factor*sin(heading0 * M_PI /180), -factor*cos(heading0 * M_PI /180));
        }

        QPointF c2(pos1);
        // stalled condition when previous boat in the back
        qreal angle1 = fmod(theta- heading1 +360, 360);
        bool stalled1 = ((angle1 >= 90) && (angle1 <= 270)) ;
        if (!stalled1) {
            c2 -= QPointF(factor*sin(heading1 * M_PI /180), -factor*cos(heading1 * M_PI /180));
        }

        path.cubicTo(c1, c2, pos1);
        oldBoat->setPath(path);
        oldBoat = boat;
        wholePath.cubicTo(c1, c2, pos1);
        pos0 = pos1;
        heading0 = heading1;
        path = QPainterPath();
        i++;
    }
    m_path = wholePath;
    emit trackChanged(track);
}

void TrackModel::setSelected(bool selected) {
    foreach(BoatModel *boat, m_boats) {
        m_situation->addSelectedBoat(boat);
    }

    emit trackSelected(selected);
}

#ifdef QML
QQmlListProperty<BoatModel> TrackModel::boatList() {
    return QQmlListProperty<BoatModel>(this, m_boats);
}
#endif
