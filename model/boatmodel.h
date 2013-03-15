//
// C++ Interface: boatmodel
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
#ifndef BOATMODEL_H
#define BOATMODEL_H

#include <QtGui>

#include "boats.h"
#include "positionmodel.h"
#include "situationmodel.h"

class SituationModel;
class TrackModel;

/**
    \class BoatModel

    \brief The Model for a Boat Position

    The class represents  the Model for a Boat, according to an
    Observer Pattern.

    BoatModel inherits PositionModel and contains data which represents
    one boat at a given time, like its position and heading.

    It shall not be mistaken with TrackModel, which holds the list of all
    positions where a Boat has navigated.
    More exactly, a TrackModel will hold a List of BoatModels.

    \sa SituationModel, TrackModel

*/

class BoatModel : public PositionModel {
        Q_OBJECT
        Q_PROPERTY(QPointF pos READ position WRITE setPosition)
        Q_PROPERTY(bool laylines READ laylines() WRITE setLaylines)
        Q_PROPERTY(qreal heading READ heading WRITE setHeading)
        Q_PROPERTY(qreal wind READ wind WRITE setWind)
        Q_PROPERTY(qreal trimSailAngle READ trimmedSailAngle WRITE setTrimmedSailAngle)
        Q_PROPERTY(bool spin READ spin WRITE setSpin)
        Q_PROPERTY(QString text READ text WRITE setText)
        Q_PROPERTY(QPointF textPos READ textPosition WRITE setTextPosition)
        Q_PROPERTY(Boats::Overlaps overlap READ overlap WRITE setOverlap)
        Q_PROPERTY(Boats::Flag flag READ flag WRITE setFlag)
        Q_PROPERTY(int dim READ dim WRITE setDim)

    public:
        BoatModel(TrackModel *track, QObject *parent = 0);
        ~BoatModel();

        // Setters and Getters for Model Data
        virtual void setPosition(const QPointF& theValue);

        void setHeading(const qreal& theValue);

        qreal trim() const { return m_trim; }
        void setTrim(const qreal& theValue);

        qreal trimmedSailAngle() const { return sailAngle() + m_trim; }
        void setTrimmedSailAngle(qreal theValue);
        qreal jibTrim() const { return m_jibTrim; }
        void setJibTrim(const qreal& theValue);

        bool hasSpin() const {return m_hasSpin; }
        void setHasSpin(const bool theValue);

        bool spin() const { return m_spin; }
        void setSpin(const bool theValue);

        qreal spinTrim() const { return m_spinTrim; }
        void setSpinTrim(const qreal& theValue);

        Boats::Overlaps overlap() const {return m_overlap; }
        void setOverlap(const Boats::Overlaps theValue);

        Boats::Flag flag() const {return m_flag; }
        void setFlag(const Boats::Flag theValue);

        bool hidden() const {return m_hidden; }
        void setHidden(const bool theValue);

        Boats::Acceleration acceleration() const {return m_acceleration; }
        void setAcceleration(const Boats::Acceleration theValue);

        // Setters and Getters for Non model Data
        TrackModel* track() const { return m_track; }

        qreal sailAngle(qreal heading = -1) const;

        qreal jibAngle(qreal heading = -1) const;

        qreal spinAngle(qreal heading = -1) const;

        void setWind(qreal wind);

        void setDim(int dim);
        int dim() const { return m_dim; }

        void  setPath(QPainterPath path);
        const QPainterPath path() const { return m_path; }

    signals:
        void trimChanged(qreal trim);
        void trimmedSailAngleChanged(qreal sailAngle);
        void jibTrimChanged(qreal jibTrim);
        void trimmedJibAngleChanged(qreal jibAngle);
        void spinChanged(bool spin);
        void spinTrimChanged(qreal spinTrim);
        void trimmedSpinAngleChanged(qreal spinAngle);
        void overlapChanged(Boats::Overlaps overlap);
        void flagChanged(Boats::Flag flag);
        void hiddenChanged(bool hidden);
        void dimChanged(int dim);

    private:
        // Model Data
        /// \a m_trim holds the sailing trim of a Boat
        qreal m_trim;

        /// \a m_jibTrim holds the jib trim
        qreal m_jibTrim;

        /// \a m_spin holds whether a spinnaker is used
        bool m_spin;

        /// \a m_spinTrim holds the spinnaker trim
        qreal m_spinTrim;

        /// \a m_overlap holds whether an overlap line should be displayed
        Boats::Overlaps m_overlap;

        /// \a m_flag holds the flag to display
        Boats::Flag m_flag;

        /// \a m_hidden holds whether the boat should display the hidden symbol
        bool m_hidden;

        /// \a m_acceleration holds how the boat speed should vary for the next period
        Boats::Acceleration m_acceleration;

        // Non model Data
        /// \a m_track keeps a pointer to the TrackModel to which
        /// it belongs
        TrackModel *m_track;

        /// \a m_hasSpin holds whether this type of boat has a spinnaker
        bool m_hasSpin;

        int m_dim;

        /// \a m_path holds the QPainterPath to the next boat
        QPainterPath m_path;
};

#endif
