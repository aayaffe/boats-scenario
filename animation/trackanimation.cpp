//
// C++ Implementation: TrackAnimation
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

#include <QSequentialAnimationGroup>

#include "trackanimation.h"

#include "boatmodel.h"
#include "trackmodel.h"

#include "boatanimation.h"
#include "propertyanimation.h"

TrackAnimation::TrackAnimation(TrackModel *track, BoatModel *boat, QObject *parent)
    : QParallelAnimationGroup(parent),
    m_track(track),
    m_boat(boat)
{

    int size = m_track->size();
    if (size < 0) {
        return;
    }

    QSequentialAnimationGroup *boatsAnimation = new QSequentialAnimationGroup(this);

    // through all boats of the track
    for (int i=0; i< size; i++) {
        if (i<size-1) {
            BoatAnimation *boatAnimation = new BoatAnimation(m_track, m_boat, i, this);
            boatsAnimation->addAnimation(boatAnimation);
        }

        QSequentialAnimationGroup *visibilityAnimation = new QSequentialAnimationGroup(this);
        PropertyAnimation *invisibletAnimation = new PropertyAnimation(m_track->boats()[i],"dim", this);
        invisibletAnimation->setDuration(2000*i);
        invisibletAnimation->setStartValue(0);
        invisibletAnimation->setEndValue(0);
        visibilityAnimation->addAnimation(invisibletAnimation);
        PropertyAnimation *dimAnimation = new PropertyAnimation(m_track->boats()[i],"dim", this);
        if (i<size-1) {
            dimAnimation->setDuration(2000);
        } else {
            dimAnimation->setDuration(0);
        }
        dimAnimation->setStartValue(255);
        dimAnimation->setEndValue(64);
        visibilityAnimation->addAnimation(dimAnimation);
        addAnimation(visibilityAnimation);
    }

    addAnimation(boatsAnimation);

    // dim all track boats
    foreach(BoatModel *boat, m_track->boats()) {
        boat->setDim(0);
    }
}

/**
    Resets the color and List of BoatModels in the track
*/

TrackAnimation::~TrackAnimation() {
    // undim all track boats
    foreach(BoatModel *boat, m_track->boats()) {
        boat->setDim(255);
    }
    m_track->changingTrack(m_track);
}
