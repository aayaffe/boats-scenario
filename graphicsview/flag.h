//
// C++ Interface: FlagGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2009 Thibaut GRIDEL
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
#ifndef FLAG_H
#define FLAG_H

#include <QGraphicsItem>

#include "boats.h"

/**
    \class FlagGraphicsItem

    \brief the QGraphicsItem for a flag

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsItem for Item framework. It displays the
    representation of a Flag on a SituationView.

    \sa SituationView, SituationScene,

*/

class FlagGraphicsItem : public QGraphicsItem {
    public:
        FlagGraphicsItem(QGraphicsItem *parent = 0);
        virtual ~FlagGraphicsItem();

        QRectF boundingRect() const;
        QPainterPath shape() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget);

        void setDisplayFlag(Boats::Flag value) {m_flag = value; update();}
        void setRect(QRectF value) { prepareGeometryChange(); m_rect = value; update();}

    private:

        /// \a m_flag holds the flag to display
        Boats::Flag m_flag;


        QRectF m_rect;
};

#endif // FLAG_H
