//
// C++ Interface: SplineAnimation
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
#ifndef SPLINEANIMATION_H
#define SPLINEANIMATION_H

#include <QPropertyAnimation>
#include <QPainterPath>

/**
    \class SplineAnimation

    \brief Animation for the position along a spline

    The class is the animation helper for a position along a spline, according
    to the Animation Framework.

    It inherits QPropertyAnimation and provides animation for a QPointF
    to BoatGraphicsItem elements, like boat position along its track, so that
    the boat moves on its track

    \sa BoatGraphicsItem
*/

class SplineAnimation : public QPropertyAnimation
{
    public:
        SplineAnimation(QObject *target, const QByteArray &propertyName, const QPainterPath &path, QObject *parent = 0);

        virtual void updateCurrentValue ( const QVariant & value );

        virtual QVariant interpolated ( const QVariant & from, const QVariant & to, qreal progress ) const;

    private:
        QPainterPath m_path;
};

#endif // SPLINEANIMATION_H
