//
// C++ Implementation: LaylinesGraphicsItem
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
#include <iostream>

#include <QPainter>
#include <QGraphicsScene>

#include "laylines.h"

#include "commontypes.h"
#include "situationmodel.h"

extern int debugLevel;

LaylinesGraphicsItem::LaylinesGraphicsItem(PositionModel *model, QGraphicsItem *parent)
        : QGraphicsPathItem(parent),
        m_model(model),
        m_length(model->situation()->situationLength()),
        m_boatLength(Boats::seriesSizeList()[model->situation()->situationSeries()]),
        m_laylineAngle(model->situation()->laylineAngle()) {
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemStacksBehindParent);
    setPen(Qt::DashLine);

    updatePath();
    setVisible(m_model->laylines());
    setWind(model->wind());

    connect(m_model, SIGNAL(laylinesChanged(bool)),
            this, SLOT(setVisible(bool)));
    connect(m_model, SIGNAL(windChanged(qreal)),
            this, SLOT(setWind(qreal)));
    connect(m_model->situation(), SIGNAL(lengthChanged(int)),
            this, SLOT(setLength(int)));
    connect(m_model->situation(), SIGNAL(laylineChanged(int)),
            this, SLOT(setLaylineAngle(int)));
    connect(m_model->situation(), SIGNAL(seriesChanged(int)),
            this, SLOT(setSeries(int)));
}


LaylinesGraphicsItem::~LaylinesGraphicsItem() {}

void LaylinesGraphicsItem::setLength(int value) {
    if (m_length != value) {
        prepareGeometryChange();
        m_length = value;
        updatePath();
    }
}

void LaylinesGraphicsItem::setSeries(int value) {
    int boatLength = Boats::seriesSizeList()[value];
    if (m_boatLength != boatLength) {
        m_boatLength = boatLength;
        updatePath();
    }
}

void LaylinesGraphicsItem::setLaylineAngle(int value) {
    if (m_laylineAngle != value) {
        m_laylineAngle = value;
        updatePath();
    }
}

void LaylinesGraphicsItem::setWind(qreal value) {
    if(m_wind != value) {
        m_wind = value;

        QTransform transform;
        transform.rotate(m_wind);
        setTransform(transform, false);
    }
}

void LaylinesGraphicsItem::setVisible(bool visible) {
    QGraphicsItem::setVisible(visible);
    // hack for performance issues. It seems an invisible path costs much
    if(m_visible != visible) {
        m_visible = visible;
        updatePath();
    }
}


void LaylinesGraphicsItem::updatePath() {
    QPainterPath path;

    int r = 1.25 * m_length * m_boatLength;
    qreal theta = m_laylineAngle * M_PI /180;

    if(m_visible) {
        path.lineTo(r*sin(theta), r*cos(theta)); // starboard layline
        path.moveTo(0, 0);
        path.lineTo(-r*sin(theta), r*cos(theta)); // port layline
    }
    setPath(path);
}
