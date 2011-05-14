//
// C++ Interface: TrackAnimation
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2010 Thibaut GRIDEL
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
#ifndef TRACKANIMATION_H
#define TRACKANIMATION_H

#include <QParallelAnimationGroup>

class BoatModel;
class TrackModel;

/**
    \class TrackAnimation

    \brief Animation for the whole track of a boat

    The class is the animation group for a track, according
    to the Animation Framework.

    It inherits QParallelAnimationGroup and contains various animation class
    for the different properties of a boat to animate along the track

    \sa BoatAnimation
*/

class TrackAnimation: public QParallelAnimationGroup
{
public:
    TrackAnimation(TrackModel *track, BoatModel *boat, QObject *parent = 0);
    ~TrackAnimation();

    BoatModel *boat() const {return m_boat; }

private:

    /// \a m_track holds the reference to the track
    TrackModel *m_track;

    /// \a m_track holds the reference to the animated boat
    BoatModel *m_boat;
};

#endif // TRACKANIMATION_H
