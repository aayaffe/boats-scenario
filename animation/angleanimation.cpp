//
// C++ Implementation: AngleAnimation
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
#include "angleanimation.h"

AngleAnimation::AngleAnimation(QObject *target, const QByteArray &propertyName, QObject *parent)
    : PropertyAnimation(target, propertyName, parent)
{
}

QVariant AngleAnimation::interpolated ( const QVariant & from, const QVariant & to, qreal progress ) const
{
    qreal valueBefore = from.toReal();
    qreal valueAfter = to.toReal();

    qreal minValue = qMin<qreal>(valueBefore,valueAfter);
    qreal maxValue = qMax<qreal>(valueBefore,valueAfter);
    if (maxValue - minValue <= 360 + minValue - maxValue) {
        // Do a simple linear interpolation.
        return valueBefore + (valueAfter - valueBefore) * progress;
    } else {
        // Do a reverse linear interpolation.
        if (valueBefore > valueAfter)
            return valueBefore + (360 + valueAfter - valueBefore) * progress;
        else
            return valueBefore + (valueAfter - valueBefore - 360) * progress;
    }
}
