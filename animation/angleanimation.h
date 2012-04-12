//
// C++ Interface: AngleAnimation
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
#ifndef ANGLEANIMATION_H
#define ANGLEANIMATION_H

#include "propertyanimation.h"

/**
    \class AngleAnimation

    \brief Animation for angles with correct rotation

    The class is the animation helper for an angle, according to the
    Animation Framework.

    It inherits QVariantAnimation and provides proper angle animation
    to BoatGraphicsItem elements, like heading or sail position, so that
    the shortest angle is taken between two options

    \sa BoatGraphicsItem
*/

class AngleAnimation : public PropertyAnimation
{
public:
    AngleAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = 0);

    virtual QVariant interpolated ( const QVariant & from, const QVariant & to, qreal progress ) const;
};

#endif // ANGLEANIMATION_H
