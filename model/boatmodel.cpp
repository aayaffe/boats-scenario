//
// C++ Implementation: boatmodel
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

#include "boatmodel.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"

extern int debugLevel;

BoatModel::BoatModel(TrackModel* track, QObject *parent)
        : PositionModel(track->situation(), parent),
        m_trim(0),
        m_jibTrim(0),
        m_spin(false),
        m_spinTrim(0),
        m_overlap(Boats::none),
        m_flag(Boats::noFlag),
        m_hidden(false),
        m_acceleration(Boats::constant),
        m_track(track),
        m_hasSpin(false),
        m_maxNormalSailAngle(0),
        m_maxNormalJibAngle(0),
        m_maxWithSpinSailAngle(0),
        m_maxWithSpinJibAngle(0),
        m_dim(255) {
    if (debugLevel & 1 << MODEL) std::cout << "new Boat " << this << std::endl;
    setOrder(track->size()+1);
}

BoatModel::~BoatModel() {
    if (debugLevel & 1 << MODEL) std::cout << "delete Boat " << this << std::endl;
}

void BoatModel::setHeading(const qreal& theValue) {
    if (theValue != m_heading) {
        m_heading = fmod(theValue,360.0); // This is always between -360 and +360
        if (m_heading < 0) m_heading += 360.0; // This ensures it is between 0 and +360
        emit headingChanged(m_heading);
        qreal trimmedSail = trimmedSailAngle();
        qreal trimmedJib = trimmedJibAngle();
        qreal trimmedSpin = trimmedSpinAngle();
        qreal trimmedGenn = trimmedGennAngle();
        emit trimmedSailAngleChanged(trimmedSail);
        emit trimmedJibAngleChanged(trimmedJib);
        emit trimmedSpinAngleChanged(trimmedSpin);
        emit trimmedGennAngleChanged(trimmedGenn);
        m_track->changingTrack(m_track);
        if (debugLevel & 1 << MODEL) std::cout
                << "heading = " << theValue
                << " sail = " << trimmedSail
                << " jib = " << trimmedJib
                << " spin = " << trimmedSpin
                << " genn = " << trimmedGenn
                << std::endl;
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
        qreal trimmedSail = trimmedSailAngle();
        emit trimmedSailAngleChanged(trimmedSail);
        if (debugLevel & 1 << MODEL) std::cout
                << "trim = " << theValue
                << " heading = " << m_heading
                << " sail = " << trimmedSail
                << std::endl;
    }
}

qreal BoatModel::trimmedSailAngle() const {
    return sailAngle() + m_trim;
}

void BoatModel::setTrimmedSailAngle(qreal theValue) {
    emit trimmedSailAngleChanged(theValue);
}

void BoatModel::setJibTrim(const qreal& theValue) {
    qreal newAngle = jibAngle() + theValue;
    if (theValue != m_jibTrim
        && newAngle < 180
        && newAngle > -180) {
        m_jibTrim = theValue;
        qreal trimmedJib = trimmedJibAngle();
        emit trimmedJibAngleChanged(trimmedJib);
        if (debugLevel & 1 << MODEL) std::cout
                << "jibtrim = " << theValue
                << " heading = " << m_heading
                << " jib = " << trimmedJib
                << std::endl;
    }
}

qreal BoatModel::trimmedJibAngle() const {
    return jibAngle() + m_jibTrim;
}

void BoatModel::setTrimmedJibAngle(qreal theValue) {
    emit trimmedJibAngleChanged(theValue);
}

void BoatModel::setHasSpin(const bool theValue) {
    if (theValue != m_hasSpin) {
        m_hasSpin = theValue;
    }
}

void BoatModel::setSpin(const bool theValue) {
    if (theValue != m_spin) {
        m_spin = theValue;
        emit spinChanged(m_spin);
        emit trimmedSailAngleChanged(trimmedSailAngle());
        emit trimmedJibAngleChanged(trimmedJibAngle());
    }
}

void BoatModel::setSpinTrim(const qreal& theValue) {
// Spinnaker graphics code doesn't actually respect spin trim so use this for gennaker trimming
    qreal sailAngle = gennAngle(); // spinAngle();
    qreal newAngle = sailAngle + theValue;
    if (theValue != m_spinTrim
        && newAngle < 180
        && newAngle > -180) {
        m_spinTrim = theValue;
        qreal trimmedSpin = trimmedSpinAngle();
        qreal trimmedGenn = trimmedGennAngle();
        emit trimmedSpinAngleChanged(trimmedSpin);
        emit trimmedGennAngleChanged(trimmedGenn);
        if (debugLevel & 1 << MODEL) std::cout
                << "spintrim = " << theValue
                << " heading = " << m_heading
                << " spin = " << trimmedSpin
                << " genn = " << trimmedGenn
                << std::endl;
    }
}

qreal BoatModel::trimmedSpinAngle() const {
    return spinAngle() + m_spinTrim;
}

void BoatModel::setTrimmedSpinAngle(qreal theValue) {
    emit trimmedSpinAngleChanged(theValue);
}

qreal BoatModel::trimmedGennAngle() const {
    return gennAngle() + m_spinTrim;
}

void BoatModel::setTrimmedGennAngle(qreal theValue) {
    emit trimmedGennAngleChanged(theValue);
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

void BoatModel::setHidden(const  bool theValue) {
    if (theValue != m_hidden) {
        if (debugLevel & 1 << MODEL) std::cout
                << "hidden = " << theValue  << std::endl;
        m_hidden = theValue;
        emit hiddenChanged(m_hidden);
    }
}

void BoatModel::setAcceleration(const  Boats::Acceleration theValue) {
    if (theValue != m_acceleration) {
        if (debugLevel & 1 << MODEL) std::cout
                << "acceleration = " << theValue  << std::endl;
        m_acceleration = theValue;
    }
}

qreal BoatModel::sailAngle(qreal heading) const {
    qreal layline = situation()->laylineAngle();
    qreal sailAngle;

    if (heading == -1) {
        heading = fmod(m_heading - wind() + 360, 360);
    }
    // within 10° inside layline angle, the sail is headed
    if (heading < layline-10) {
        sailAngle =  fmin(layline-20,heading); // layline-20 so that sail is at stall angle of 10 degrees when heading = layline-10
    } else if (heading > 360 - (layline-10)) {
        sailAngle =  fmax(-(layline-20),heading - 360);
    } else {
        qreal maxSailAngle;
        if (m_hasSpin && m_spin) {
            maxSailAngle = m_maxWithSpinSailAngle;
        } else {
            maxSailAngle = m_maxNormalSailAngle;
        }

        // linear incidence variation
        // incidence is 15° at layline angle and maxWithSpinSailAngle downwind
        qreal a = (180 - layline) / (maxSailAngle - 15);
        qreal b = layline / a - 15;
        if (heading<180) {
            sailAngle = heading/a - b;
        } else {
            sailAngle = heading/a - b - 2*maxSailAngle;
        }
    }
    return sailAngle;
}

qreal BoatModel::jibAngle(qreal heading) const {
    qreal layline = situation()->laylineAngle();
    qreal sailAngle;

    if (heading == -1) {
        heading = fmod(m_heading - wind() + 360, 360);
    }
    // within 10° inside layline angle, the sail is headed
    if (heading < layline-10) {
        sailAngle =  fmin(layline-20,heading); // layline-20 so that sail is at stall angle of 10 degrees when heading = layline-10
    } else if (heading > 360 - (layline-10)) {
        sailAngle =  fmax(-(layline-20),heading - 360);
    } else {
        qreal maxJibAngle;
        if (m_hasSpin && m_spin) {
            maxJibAngle = m_maxWithSpinJibAngle;
        } else {
            maxJibAngle = m_maxNormalJibAngle;
        }
        // linear incidence variation
        // incidence is 20° at layline angle and maxWithSpinJibAngle downwind
        qreal a = (180 - layline) / (maxJibAngle - 20);
        qreal b = layline / a - 20;
        if (heading<180) {
            sailAngle = heading/a - b;
        } else {
            sailAngle = heading/a - b - 2*maxJibAngle;
        }
    }

    return sailAngle;
}

qreal BoatModel::spinAngle(qreal heading) const {
    qreal sailAngle;

    if (heading == -1) {
        heading = fmod(m_heading - wind() + 360, 360);
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

    return sailAngle;
}

qreal BoatModel::gennAngle(qreal heading) const {
    qreal sailAngle;

    if (heading == -1) {
        heading = fmod(m_heading - wind() + 360, 360);
    }
    // within 10° above downwind angle, the sail is headed
    if (heading < 80) {
        sailAngle = heading / 80 * 20;
    } else if (heading > 360 - 80) {
        sailAngle = - (360 - heading) / 80 * 20;
    } else {
        // linear incidence variation
        // incidence is 20° at 80° heading and 30° downwind
        qreal a = (180 - 80) / (30 - 20);
        qreal b = 80 / a - 20;
        if (heading<180) {
            sailAngle = heading/a - b;
        } else {
            sailAngle = heading/a - b - 2*30;
        }
    }

    return sailAngle;
}

void BoatModel::setWind(qreal wind) {
    PositionModel::setWind(wind);
    emit headingChanged(m_heading);
    emit trimmedSailAngleChanged(trimmedSailAngle());
    emit trimmedJibAngleChanged(trimmedJibAngle());
    emit trimmedSpinAngleChanged(trimmedSpinAngle());
    emit trimmedGennAngleChanged(trimmedGennAngle());
}

void BoatModel::setDim(int dim) {
    m_dim = dim;
    emit dimChanged(dim);
}

void BoatModel::setPath(QPainterPath path) {
    m_path = path;
}
