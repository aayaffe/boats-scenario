//
// C++ Interface: positionmodel
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
#ifndef POSITIONMODEL_H
#define POSITIONMODEL_H

#include <QtGui>

class SituationModel;

/**
    \class PositionModel

    \brief The Model for a Position

    The class represents the Model for a Position, according to an
    Observer Pattern.

    PositionModel contains data which represents an object with a position,
    and a stacking order. It is inherited by BoatModel and PointModel.

    \sa SituationModel, BoatModel, PointModel

*/

class PositionModel : public QObject {
        Q_OBJECT
    public:
        PositionModel(SituationModel* situation, QObject *parent = 0);
        virtual ~PositionModel();

        // Setters and Getters for Model Data
        QPointF position() const { return m_position; }
        virtual void setPosition(const QPointF& theValue);

        int order() const { return m_order; }
        void setOrder(const int theValue);

        qreal heading() const { return m_heading; }
        virtual void setHeading(const qreal& theValue);

        QString text() const {return m_text; }
        void setText(const QString theValue);

        QPointF textPosition() const {return m_textPosition; }
        void setTextPosition(const QPointF& theValue);

        bool laylines() const {return m_laylines; }
        void setLaylines(bool theValue);

        // Setters and Getters for Non model Data
        SituationModel* situation() const { return m_situation; }

        virtual qreal wind() const;

        QStringList discardedXml() const { return m_discardedXml; }
        void appendDiscardedXml(const QString& theValue);

    public slots:
        virtual void setWind(qreal wind);

    signals:
        void positionChanged(QPointF position);
        void orderChanged(int order);
        void headingChanged(qreal heading);

        void textChanged(QString text);
        void textPositionChanged(QPointF textPosition);
        void windChanged(qreal wind);
        void laylinesChanged(bool laylines);

    protected:
        // Model Data
        /// \a m_position holds the position of the object
        QPointF m_position;

        /// \a m_order holds the stacking order of the object. It starts at 1 for track boat
        int m_order;

        /// \a m_heading holds the heading of the object
        qreal m_heading;

        /// \a m_text holds the text to display
        QString m_text;

        /// \a m_textPosition holds the position of the text to display
        QPointF m_textPosition;

        /// \a m_laylines holds wether the object should display laylines
        bool m_laylines;

        // Non model Data
        /// \a m_situation keeps a pointer to the SituationModel to which
        /// it belongs
        SituationModel *m_situation;

        qreal m_wind;

        /// \a m_discardedXml keeps all unparsed xml tags
        QStringList m_discardedXml;
};

#endif
