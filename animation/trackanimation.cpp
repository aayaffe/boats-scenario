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

    int size = m_track->size() - 1;
    if (size < 0) {
        return;
    }

    QSequentialAnimationGroup *boatsAnimation = new QSequentialAnimationGroup(this);

    // through all boats of the track
    for (int i=0; i< size; i++) {
        BoatAnimation *boatAnimation = new BoatAnimation(m_track, m_boat, i, this);
        boatsAnimation->addAnimation(boatAnimation);
        PropertyAnimation *visibleboatAnimation = new PropertyAnimation(m_track->boats()[i],"visible", this);
        visibleboatAnimation->setDuration(2000 * (size-1));
        visibleboatAnimation->setStartValue(false);
        visibleboatAnimation->setKeyValueAt((qreal)i/(qreal)(size-1), true);
        visibleboatAnimation->setEndValue(true);
        addAnimation(visibleboatAnimation);
    }

    addAnimation(boatsAnimation);

    // dim all track boats
    foreach(BoatModel *boat, m_track->boats()) {
        boat->setDim(true);
        boat->setVisible(false);
        boat->track()->setShowPath(false);
    }
}

/**
    Resets the color and List of BoatModels in the track
*/

TrackAnimation::~TrackAnimation() {
    // undim all track boats
    foreach(BoatModel *boat, m_track->boats()) {
        boat->setDim(false);
        boat->setVisible(true);
        boat->track()->setShowPath(true);
    }
}
