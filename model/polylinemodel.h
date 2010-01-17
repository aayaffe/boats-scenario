//
// C++ Interface: polylinemodel
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
#ifndef POLYLINEMODEL_H
#define POLYLINEMODEL_H

#include <QtGui>

#include "boats.h"

class SituationModel;
class PointModel;

/**
    \class PolyLineModel

    \brief The Model for a list of Points

    The class represents the Model for a Line, according to an
    Observer Pattern.

    PolyLineModel contains data which describe one line, like the list of
    points.

    \sa SituationModel, PointModel

*/

class PolyLineModel : public QObject {
        Q_OBJECT
    public:
        PolyLineModel(SituationModel* situation = 0, QObject *parent = 0);
        ~PolyLineModel();

        PointModel * addPoint(PointModel *point, int order = -1);
        int deletePoint(PointModel *point);

        void displayPoints();
        void hidePoints();

    // Setters and Getters for Model Data
        int size() const { return m_points.size();}
        const QList<PointModel*> points() const { return m_points; }

        const QPainterPath path() const { return m_path; }

        // Setters and Getters for Non model Data
        SituationModel* situation() const { return m_situation; }

        QStringList discardedXml() const { return m_discardedXml; }
        void appendDiscardedXml(const QString& theValue);

        void changingPolyLine(PolyLineModel *polyline);

    signals:
        // Signals for PolyLineModel parameters
        void polyLineChanged(PolyLineModel *polyLine);

    private:
        // Model Data
        /// \a m_points holds the List of Point Positions of the PolyLine
        QList<PointModel*> m_points;

        // Non model Data
        /// \a m_situation keeps a pointer to the SituationModel to which
        /// it belongs
        SituationModel *m_situation;

        /// \a m_path holds the QPainterPath of the PolyLine
        QPainterPath m_path;

        /// \a m_discardedXml keeps all unparsed xml tags
        QStringList m_discardedXml;
};

#endif // POLYLINEMODEL_H
