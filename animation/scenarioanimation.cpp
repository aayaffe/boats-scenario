//
// C++ Implementation: ScenarioAnimation
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

#include "scenarioanimation.h"

#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "statemachine.h"

#include "trackanimation.h"
#include "angleanimation.h"

#include <QSequentialAnimationGroup>

ScenarioAnimation::ScenarioAnimation(SituationModel *situation, QObject *parent)
    : QParallelAnimationGroup(parent),
      m_situation(situation),
      m_windAnimation(new QSequentialAnimationGroup(this)) {
    addAnimation(m_windAnimation);

    connect(m_situation->stateMachine()->playState(), SIGNAL(entered()),
            this, SLOT(start()));
    connect(m_situation->stateMachine()->pauseState(), SIGNAL(entered()),
            this, SLOT(pause()));
    connect(m_situation->stateMachine()->stopState(), SIGNAL(entered()),
            this, SLOT(stop()));
}

ScenarioAnimation::~ScenarioAnimation() {
    delete m_windAnimation;
}

void ScenarioAnimation::setAnimation() {
    foreach(TrackModel *track, m_situation->tracks()) {
        BoatModel *boat = track->boats()[0];
        BoatModel *animationBoat = new BoatModel(track);
        animationBoat->setOrder(0);
        animationBoat->setPosition(boat->position());
        animationBoat->setHeading(boat->heading());
        animationBoat->setWind(m_situation->wind().windAt(0));
        animationBoat->setLaylines(boat->laylines());
        connect(&m_situation->wind(), SIGNAL(directionChanged(qreal)),
                animationBoat, SLOT(setWind(qreal)));

        TrackAnimation *animation = new TrackAnimation(track, animationBoat, this);
        addAnimation(animation);
        m_animationItems.push_back(animation);
    }

    for (int i = 0; i < m_situation->wind().size()-1; ++i) {
        AngleAnimation *wind = new AngleAnimation(&m_situation->wind(), "direction");
        wind->setDuration(2000);
        wind->setStartValue(m_situation->wind().windAt(i));
        wind->setEndValue(m_situation->wind().windAt(i+1));
        m_windAnimation->addAnimation(wind);
    }

    setCurrentTime(0);

}

void ScenarioAnimation::unsetAnimation() {
    foreach(TrackAnimation *animation, m_animationItems) {
        // the boat was never really part of the track, we use situation signal
        // directly to have the graphicsitem removed
        m_situation->removingBoat(animation->boat());
        removeAnimation(animation);
        m_animationItems.removeOne(animation);
        delete animation->boat();
        delete animation;
    }

    for (int i = 0; i < m_situation->wind().size()-1; ++i) {
        QAbstractAnimation *animation = m_windAnimation->animationAt(0);
        m_windAnimation->removeAnimation(animation);
        delete animation;
    }
    m_situation->wind().setDirection(m_situation->wind().windAt(0));


}

void ScenarioAnimation::updateCurrentTime(int currentTime)
{
    QParallelAnimationGroup::updateCurrentTime(currentTime);
    emit timeChanged(currentTime);
}
