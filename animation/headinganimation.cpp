//
// C++ Implementation: HeadingAnimation
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
#include <cmath>

#include "headinganimation.h"
#include "angleanimation.h"

HeadingAnimation::HeadingAnimation(QObject *target, const QByteArray &propertyName, const QPainterPath &path, QObject *parent)
    : PropertyAnimation(target, propertyName, parent),
      m_path(path)
{
}

QVariant HeadingAnimation::interpolated ( const QVariant & from, const QVariant & to, qreal progress ) const
{
    Q_UNUSED(from)
    Q_UNUSED(to)

    qreal length = m_path.length();
    qreal percent = m_path.percentAtLength(length * progress);

    return fmod(360+90-m_path.angleAtPercent(percent),360.0);
}
