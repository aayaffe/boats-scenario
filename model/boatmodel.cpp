//
// C++ Implementation: boatmodel
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
#include <iostream>
#include <cmath>

#include "boatmodel.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"

extern int debugLevel;

BoatModel::BoatModel(TrackModel* track, QObject *parent)
        : PositionModel(track->situation(), parent),
        m_heading(0),
        m_trim(0),
        m_spin(false),
        m_spinTrim(0),
        m_overlap(Boats::none),
        m_flag(Boats::noFlag),
        m_track(track) {
    if (debugLevel & 1 << MODEL) std::cout << "new Boat " << this << std::endl;
    setOrder(track->size()+1);
}

BoatModel::~BoatModel() {
    if (debugLevel & 1 << MODEL) std::cout << "delete Boat " << this << std::endl;
}

void BoatModel::setHeading(const qreal& theValue) {
    if (theValue != m_heading) {
        m_heading = fmod(theValue+360.0,360.0);
        emit headingChanged(m_heading);
        emit trimmedSailAngleChanged(sailAngle()+ m_trim);
        emit trimmedSpinAngleChanged(spinAngle() + m_spinTrim);
        m_track->changingTrack(m_track);
    }
}

void BoatModel::setPosition(const QPointF& theValue) {
    PositionModel::setPosition(theValue);
    m_track->changingTrack(m_track);
}

void BoatModel::setTrim(const qreal& theValue) {
    qreal newAngle = sailAngle() + theValue;
    if (theValue != m_trim
        && newAngle < 180
        && newAngle > -180) {
        m_trim = theValue;
        if (debugLevel & 1 << MODEL) std::cout
                << "trim = " << theValue  << std::endl;
        emit trimChanged(m_trim);
        emit trimmedSailAngleChanged(sailAngle()+ m_trim);
    }
}

void BoatModel::setSpin(const bool theValue) {
    if (theValue != m_spin) {
        m_spin = theValue;
        emit spinChanged(m_spin);
    }
}

void BoatModel::setSpinTrim(const qreal& theValue) {
    qreal sailAngle = spinAngle();
    qreal newAngle = sailAngle + theValue;
    if (theValue != m_spinTrim
        && newAngle < 180
        && newAngle > -180) {
        m_spinTrim = theValue;
        if (debugLevel & 1 << MODEL) std::cout
                << "spinTrim = " << theValue  << std::endl;
        emit spinTrimChanged(m_spinTrim);
        emit trimmedSpinAngleChanged(spinAngle() + m_spinTrim);
    }
}

void BoatModel::setOverlap(const  Boats::Overlaps theValue) {
    if (theValue != m_overlap) {
        if (debugLevel & 1 << MODEL) std::cout
                << "overlap = " << theValue  << std::endl;
        m_overlap = theValue;
        emit overlapChanged(m_overlap);
    }
}

void BoatModel::setFlag(const  Boats::Flag theValue) {
    if (theValue != m_flag) {
        if (debugLevel & 1 << MODEL) std::cout
                << "flag = " << theValue  << std::endl;
        m_flag = theValue;
        emit flagChanged(m_flag);
    }
}

qreal BoatModel::sailAngle(qreal heading) const {
    qreal layline = situation()->laylineAngle();
    qreal sailAngle;

    if (heading == -1) {
        heading = m_heading;
    }
    // within 10° inside layline angle, the sail is headed
    if (heading < layline-10) {
        sailAngle =  heading;
    } else if (heading > 360 - (layline-10)) {
        sailAngle =  heading - 360;
    } else {

        switch (m_track->series()) {
            // tornado has fixed 20° incidence
        case Boats::tornado:
            if (heading<180) {
                sailAngle = 20;
            } else {
                sailAngle = - 20;
            }
            break;
        default:
            // linear incidence variation
            // incidence is 15° at layline angle and 90° downwind
            qreal a = (180 - layline) / 75;
            qreal b = layline / a - 15;
            if (heading<180) {
                sailAngle = heading/a - b;
            } else {
                sailAngle = heading/a - b - 180;
            }
            break;
        }
    }

    if (debugLevel & 1 << MODEL) std::cout
            << "heading = " << heading
            << " sail = " << sailAngle
            << std::endl;
    return sailAngle;
}

qreal BoatModel::spinAngle(qreal heading) const {
    qreal sailAngle;

    if (heading == -1) {
        heading = m_heading;
    }
    // within 10° above downwind angle, the sail is headed
    if (heading < 80) {
        sailAngle =  heading;
    } else if (heading > 280) {
        sailAngle =  heading - 360;
    } else {
        if (heading<180) {
            sailAngle = heading - 20;
        } else {
            sailAngle = heading + 20;
        }
    }

    if (debugLevel & 1 << MODEL) std::cout
            << "heading = " << heading
            << " spin = " << sailAngle
            << std::endl;
    return sailAngle;
}
