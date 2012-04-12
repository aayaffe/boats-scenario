//
// C++ Interface: PropertyAnimation
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
#ifndef PROPERTYANIMATION_H
#define PROPERTYANIMATION_H

#include <QPropertyAnimation>

/**
    \class PropertyAnimation

    \brief Animation for properties irrelevant of state

    The class is the animation helper for a property, according to the
    Animation Framework.

    It inherits QVariantAnimation and provides proper property animation
    so that the property is updated whatever the state, unlike QPropertyAnimation

    \sa QPropertyAnimation
*/

class PropertyAnimation : public QPropertyAnimation
{
public:
    PropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = 0);

    virtual void updateCurrentValue ( const QVariant & value );

    virtual QVariant interpolated ( const QVariant & from, const QVariant & to, qreal progress ) const;

};

#endif // PROPERTYANIMATION_H
