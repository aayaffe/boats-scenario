//
// C++ Implementation: PolyLineGraphicsItem
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
#include <iostream>
#include <cmath>

#include <QPainter>
#include <QGraphicsScene>


#include "polyline.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "polylinemodel.h"

extern int debugLevel;

PolyLineGraphicsItem::PolyLineGraphicsItem(PolyLineModel *polyline, QGraphicsItem *parent)
        : QGraphicsPathItem(parent),
        m_polyline(polyline) {
    setZValue(0);
    setPolyLine();

    connect(polyline, SIGNAL(polyLineChanged(PolyLineModel*)),
            this, SLOT(setPolyLine()));
    connect(polyline->situation(), SIGNAL(polyLineRemoved(PolyLineModel*)),
            this, SLOT(deleteItem(PolyLineModel*)));
}


PolyLineGraphicsItem::~PolyLineGraphicsItem() {}

void PolyLineGraphicsItem::setPolyLine() {
        prepareGeometryChange();
        setPath(m_polyline->path());
        update();
}

void PolyLineGraphicsItem::deleteItem(PolyLineModel *polyline) {
    if (polyline == m_polyline) {
        if (debugLevel & 1 << VIEW) std::cout << "deleting polylineGraphics for model" << m_polyline << std::endl;
        scene()->removeItem(this);
        delete this;
    }
}

