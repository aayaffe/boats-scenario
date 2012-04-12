//
// C++ Interface: HeadingAnimation
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
#ifndef HEADINGANIMATION_H
#define HEADINGANIMATION_H

#include <QPainterPath>

#include "propertyanimation.h"

/**
    \class HeadingAnimation

    \brief Animation for angles with correct rotation

    The class is the animation helper for an angle, according to the
    Animation Framework.

    It inherits QVariantAnimation and provides proper animation
    to BoatGraphicsItem elements along a QPainterPath, like the boat heading, so
    that the heading follows the path

    \sa BoatGraphicsItem
*/

class HeadingAnimation : public PropertyAnimation
{
public:
    HeadingAnimation(QObject *target, const QByteArray &propertyName, const QPainterPath &path, QObject *parent = 0);

    virtual QVariant interpolated ( const QVariant & from, const QVariant & to, qreal progress ) const;

private:
    QPainterPath m_path;

};

#endif // HEADINGANIMATION_H
