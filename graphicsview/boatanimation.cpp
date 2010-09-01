//
// C++ Implementation: BoatAnimation
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
#include <iostream>
#include <cmath>

#include <QtGui>

#include "boatanimation.h"
#include "headinganimation.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "boat.h"

/**
    Populates the internal lists of Positions and Headings function
    of time

    This method parses the path of the \a track, does some stalling
    analysis and determines position and heading as a function of time.

    It also gives the pale color to \a track boats, while keeping
    original color for animation \a boat.
*/

BoatAnimation::BoatAnimation(TrackModel *track, BoatGraphicsItem *boat, int maxSize, QGraphicsItemAnimation *parent)
    : QGraphicsItemAnimation(parent),
    m_track(track),
    m_boat(boat),
    m_headingAnimation(new HeadingAnimation(boat->boat(), "heading")),
    m_sailAngleAnimation(new HeadingAnimation(boat->boat(), "trimSailAngle")),
    m_maxSize(maxSize),
    m_time(QTime::currentTime()) {

    QPainterPath path = m_track->path();
    int size = m_track->size() - 1;
    if (size < 0) {
        return;
    }

    m_boat->setOrder(0);
    QPointF point = path.elementAt(0);
    setPosAt(0,point);
    BoatModel *model = m_track->boats()[0];
    m_headingAnimation->setDuration(2000*m_maxSize);
    m_headingAnimation->setStartValue(model->heading());
    m_sailAngleAnimation->setDuration(2000*m_maxSize);
    m_sailAngleAnimation->setStartValue(model->trimmedSailAngle());

    // through all boats of the track
    for (int i=0; i< size; i++) {
        qreal index = 0;
        QPointF c1 = path.elementAt(i*3+1);
        QPointF c2 = path.elementAt(i*3+2);
        QPointF end = path.elementAt(i*3+3);
        bool stalled = ((point == c1) || (c2 == end));
        QPainterPath curve(point);
        curve.cubicTo(c1,c2,end);
        qreal length = curve.length();
        float e = 8;
        // through 8 intermediary points between 2 boats
        for (int j=1; j<=e; j++) {
            qreal percent = curve.percentAtLength(length*j/e);
            index = (i*e+j)/(maxSize*e);
            setPosAt(index, curve.pointAtPercent(percent));
            if (!stalled) {
                m_headingAnimation->setKeyValueAt(index, fmod(360+90-curve.angleAtPercent(percent),360.0));
            }
        }
        model = m_track->boats()[i+1];
        if (stalled) {
            m_headingAnimation->setKeyValueAt(index, model->heading());
        }
        m_sailAngleAnimation->setKeyValueAt(index, model->trimmedSailAngle());
        point = end;
    }

    // dim all track boats
    foreach(BoatModel *boat, m_track->boats()) {
        boat->setDim(true);
    }
}

/**
    Resets the color and List of BoatModels in the track
*/

BoatAnimation::~BoatAnimation() {
    while (!m_boats.isEmpty()) {
        m_track->addBoat(m_boats.last());
        m_boats.pop_back();
    }

    // undim all track boats
    foreach(BoatModel *boat, m_track->boats()) {
        boat->setDim(false);
    }
}

/**
    Updates TrackModel and animation boat position and heading
    at \a step time, with a rate limit of one refresh every 40ms
*/

void BoatAnimation::afterAnimationStep(qreal step) {
    // limit update rate to 40ms
    if ((m_time.elapsed() < 40) && (step != 0) && (step != 1)) {
        return;
    }

    m_boat->boat()->setPosition(posAt(step));

    m_headingAnimation->setCurrentTime(step*2000*m_maxSize);
    m_sailAngleAnimation->setCurrentTime(step*2000*m_maxSize);

    int index = floor(step * m_maxSize);
    BoatModel *boat;
    for (int i=m_track->size()-1; i > index; i--) {
        boat = m_track->boats()[i];
        m_boats.push_back(boat);
        m_track->deleteBoat(boat);
    }
    if (!m_boats.isEmpty()) {
        for (int i = m_track->size()-1; i < index; i++) {
            m_track->addBoat(m_boats.last());
            m_boats.pop_back();
        }
    }

    if (index > m_track->size() - 1) {
        return;
    }

    boat = m_track->boats()[index];
    m_boat->boat()->setOverlap(boat->overlap());
    m_boat->boat()->setFlag(boat->flag());
    m_boat->boat()->setText(boat->text());
    m_boat->boat()->setTextPosition(boat->textPosition());
    m_boat->boat()->setSpin(boat->spin());

    // trigger next update rate calculation
    m_time.start();
}
