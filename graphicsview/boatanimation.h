//
// C++ Interface: BoatAnimation
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
#ifndef BOATANIMATION_H
#define BOATANIMATION_H

#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QTime>

class BoatGraphicsItem;
class TrackModel;
class BoatModel;
class HeadingAnimation;

/**
    \class BoatAnimation

    \brief The animation Item for a scenario

    The class is the animation helper for a Scenario, according to the
    Graphics View Framework.

    It inherits QGraphicsItemAnimation and provides BoatGraphicsItem
    specific animation features, like position and heading given a
    time step.

    It also does a rate limiting update for drawing as Windows platforms
    suffer from that.

    \sa SituationScene, SituationModel
*/

class BoatAnimation : public QParallelAnimationGroup {
    public:
        BoatAnimation(TrackModel *track, BoatGraphicsItem *boat, int maxSize,  QObject *parent = 0);
        ~BoatAnimation();
        BoatGraphicsItem *boat() const {return m_boat; }

    protected:
        virtual void afterAnimationStep(qreal step);

    private:

        /// \a m_track holds the reference to the track
        TrackModel *m_track;

        /// \a m_boats holds the List of boats hidden during animation
        QList<BoatModel*> m_boats;

        /// \a m_boat holds the reference to the animation BoatGraphicsItem
        BoatGraphicsItem *m_boat;

        HeadingAnimation *m_headingAnimation;
        HeadingAnimation *m_sailAngleAnimation;

        QSequentialAnimationGroup *m_posAnimation;

        /// \a m_maxSize holds the maximum track size of the scenario
        int m_maxSize;

        /// \a m_time holds the timer used for movement filtering
        QTime m_time;
};

#endif
