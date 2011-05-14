//
// C++ Implementation: PropertyAnimation
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
#include <QVariantAnimation>

#include "propertyanimation.h"

#include "boats.h"

PropertyAnimation::PropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent)
    : QPropertyAnimation(target, propertyName, parent)
{
}

void PropertyAnimation::updateCurrentValue ( const QVariant & value )
{
//    doesn't work because QPropertyAnimationPrivate::updateProperty checks for not Stopped
//    QPropertyAnimation::updateCurrentValue(value);
    targetObject()->setProperty(propertyName(), value);
}


QVariant boolInterpolator(const bool &start, const bool &end, qreal progress)
{
    Q_UNUSED(end)
    Q_UNUSED(progress)
    return start;
}

//qRegisterAnimationInterpolator<bool>(boolInterpolator);

QVariant qstringInterpolator(const QString &start, const QString &end, qreal progress)
{
    Q_UNUSED(end)
    Q_UNUSED(progress)
    return start;
}

//qRegisterAnimationInterpolator<QString>( qstringInterpolator );


QVariant PropertyAnimation::interpolated ( const QVariant & from, const QVariant & to, qreal progress ) const
{
    if (strcmp(from.typeName(), "bool") == 0) {
        return boolInterpolator(from.toBool(), to.toBool(), progress);
    }
    else if (strcmp(from.typeName(), "QString") == 0) {
        return qstringInterpolator(from.toString(), to.toString(), progress);
    }
    return QVariantAnimation::interpolated(from, to, progress);
}

