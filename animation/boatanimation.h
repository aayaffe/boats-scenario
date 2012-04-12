//
// C++ Interface: BoatAnimation
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
#ifndef BOATANIMATION_H
#define BOATANIMATION_H

#include <QParallelAnimationGroup>
#include <QEasingCurve>

class TrackModel;
class BoatModel;

/**
    \class BoatAnimation

    \brief The animation Item for a boat in its track

    The class is the animation group for a boat, according
    to the Animation Framework.

    It inherits QParallelAnimationGroup and provides all animation features,
    like position and heading given a time step.

    \sa SituationScene, SituationModel
*/

class BoatAnimation : public QParallelAnimationGroup {
    public:
        BoatAnimation(TrackModel *track, BoatModel *boat, int index,  QObject *parent = 0);
        ~BoatAnimation();
        BoatModel *boat() const {return m_boat; }

        void setEasingCurve ( const QEasingCurve & easing );

    private:
        /// \a m_track holds the reference to the track
        TrackModel *m_track;

        /// \a m_boat holds the reference to the boat
        BoatModel *m_boat;
};

#endif
