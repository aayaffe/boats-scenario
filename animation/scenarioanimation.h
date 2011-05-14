//
// C++ Interface: ScenarioAnimation
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
#ifndef SCENARIOANIMATION_H
#define SCENARIOANIMATION_H

#include <QParallelAnimationGroup>

/**
    \class ScenarioAnimation

    \brief Animation for the whole scenario

    The class is the animation group for a scenario, according
    to the Animation Framework.

    It inherits QParallelAnimationGroup and contains TrackAnimations
    for the different tracks of the scenario

    \sa TrackAnimation
*/

class ScenarioAnimation: public QParallelAnimationGroup
{
    Q_OBJECT

    public:
        virtual void updateCurrentTime(int currentTime);

    signals:
        void timeChanged(int time);

};

#endif // SCENARIOANIMATION_H
