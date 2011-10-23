//
// C++ Implementation: BoatAnimation
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

#include "boatanimation.h"

#include "boats.h"

#include "angleanimation.h"
#include "headinganimation.h"
#include "situationmodel.h"
#include "splineanimation.h"

#include "trackmodel.h"
#include "boatmodel.h"

/**
    Populates the animation items for position, Headings and other attributes.

    This method parses the path of the \a track, does some stalling
    analysis and determines specific position animation.
*/

BoatAnimation::BoatAnimation(TrackModel *track, BoatModel *boat, int index, QObject *parent)
    : QParallelAnimationGroup(parent),
    m_track(track),
    m_boat(boat)
{

    BoatModel *model = track->boats()[index];
    QPainterPath curve = model->path();
    QPointF point = curve.elementAt(0);
    QPointF c1 = curve.elementAt(1);
    QPointF c2 = curve.elementAt(2);
    QPointF end = curve.elementAt(3);
    bool stalled = ((point == c1) || (c2 == end));

    // position
    SplineAnimation *splineAnimation = new SplineAnimation(m_boat, "pos", model->path());
    splineAnimation->setDuration(2000);
    splineAnimation->setStartValue(point);
    splineAnimation->setEndValue(end);
    addAnimation(splineAnimation);

    // text
    PropertyAnimation *textAnimation = new PropertyAnimation(boat, "text");
    textAnimation->setDuration(2000);
    textAnimation->setStartValue(model->text());
    textAnimation->setEndValue(model->text());
    addAnimation(textAnimation);

    // text position
    PropertyAnimation *textPosAnimation = new PropertyAnimation(boat, "textPos");
    textPosAnimation->setDuration(2000);
    textPosAnimation->setStartValue(model->textPosition());
    textPosAnimation->setEndValue(model->textPosition());
    addAnimation(textPosAnimation);

    // heading
    QVariantAnimation *headingAnimation;
    if (stalled) {
        headingAnimation = new AngleAnimation(boat, "heading");
    } else {
        headingAnimation = new HeadingAnimation(boat, "heading", model->path());
    }
    headingAnimation->setDuration(2000);
    headingAnimation->setStartValue(model->heading());
    addAnimation(headingAnimation);

    // sail angle
    AngleAnimation *sailAngleAnimation = new AngleAnimation(boat, "trimSailAngle");
    sailAngleAnimation->setDuration(2000);
    sailAngleAnimation->setStartValue(model->trimmedSailAngle());
    addAnimation(sailAngleAnimation);

    // spin
    PropertyAnimation *spinAnimation = new PropertyAnimation(boat, "spin");
    spinAnimation->setDuration(2000);
    spinAnimation->setStartValue(model->spin());
    addAnimation(spinAnimation);

    // overlap
    PropertyAnimation *overlapAnimation = new PropertyAnimation(boat, "overlap");
    overlapAnimation->setDuration(2000);
    overlapAnimation->setStartValue(QVariant(model->overlap()));
    overlapAnimation->setEndValue(QVariant(model->overlap()));
    addAnimation(overlapAnimation);

    // flag
    PropertyAnimation *flagAnimation = new PropertyAnimation(boat, "flag");
    flagAnimation->setDuration(2000);
    flagAnimation->setStartValue(model->flag());
    flagAnimation->setEndValue(model->flag());
    addAnimation(flagAnimation);

    // acceleration
    switch(model->acceleration()) {
        case Boats::accelerating:
            setEasingCurve(QEasingCurve::InSine);
            break;
        case Boats::decelerating:
            setEasingCurve(QEasingCurve::OutSine);
            break;
        default:
            break;
    }

    // end values that need next boat position
    model = m_track->boats()[index+1];
    headingAnimation->setEndValue(model->heading());
    sailAngleAnimation->setEndValue(model->trimmedSailAngle());
    spinAnimation->setEndValue(model->spin());
}

BoatAnimation::~BoatAnimation() {
}

void BoatAnimation::setEasingCurve(const QEasingCurve &easing) {
    for (int i=0; i< animationCount(); ++i) {
        QVariantAnimation *animation = dynamic_cast<QVariantAnimation*>(animationAt(i));
        if(animation) {
            animation->setEasingCurve(easing);
        }
    }
}
