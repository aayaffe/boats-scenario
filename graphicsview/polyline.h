//
// C++ Interface: PolyLineGraphicsItem
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2009-2010 Thibaut GRIDEL
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
#ifndef POLYLINE_H
#define POLYLINE_H

#include <QGraphicsItem>
#include <QObject>
#include <QColor>

class PolyLineModel;

/**
    \class PolyLineGraphicsItem

    \brief the QGraphicsItem for a polyline

    The class represents the Item according to the Graphics View Framework.

    It inherits QGraphicsItem for Item framework and QObject for slot and
    signal framework. It displays the representation of a PolyLineModel on a
    SituationView.

    \sa SituationView, SituationScene, SituationModel, PolyLineModel

*/

class PolyLineGraphicsItem : public QObject, public QGraphicsPathItem {
        Q_OBJECT
    public:
        PolyLineGraphicsItem(PolyLineModel *polyline = 0, QGraphicsItem *parent = 0);
        ~PolyLineGraphicsItem();

        PolyLineModel* polyline() const { return m_polyline; }

    public slots:
        void setPolyLine();
        void deleteItem(PolyLineModel *polyline);

    private:

        /// \a m_polyline holds the PolyLineModel being represented
        PolyLineModel *m_polyline;
};

#endif
