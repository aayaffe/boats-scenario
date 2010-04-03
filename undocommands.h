//
// C++ Interface: undocommands
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
#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H

#include <QUndoCommand>
#include <QPointF>

#include "boats.h"

class SituationModel;
class TrackModel;
class PositionModel;
class BoatModel;
class MarkModel;
class PolyLineModel;
class PointModel;

enum {
    SET_TITLE,
    SET_RULES,
    SET_SHOWLAYLINE,
    SET_LAYLINE,
    SET_SITUATIONSERIES,
    SET_ABSTRACT,
    SET_DESCRIPTION,
    SET_SERIES,
    SET_COLOR,
    SET_SHOWPATH,
    MOVE_MODEL,
    HEADING_BOAT,
    OVERLAP_BOAT,
    FLAG_BOAT,
    TRIM_BOAT,
    SPIN_BOAT,
    SET_TEXT,
    MOVE_TEXT,
    ZONE_MARK,
    LENGTH_MARK
};

class SetTitleUndoCommand : public QUndoCommand {

    public:
        SetTitleUndoCommand(SituationModel* situation, QString title, QUndoCommand *parent = 0);
        ~SetTitleUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_TITLE; }

    private:
        SituationModel *m_situation;
        QString m_oldTitle;
        QString m_newTitle;
};

class SetRulesUndoCommand : public QUndoCommand {

    public:
        SetRulesUndoCommand(SituationModel* situation, QString rules, QUndoCommand *parent = 0);
        ~SetRulesUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_RULES; }

    private:
        SituationModel *m_situation;
        QString m_oldRules;
        QString m_newRules;
};

class SetShowLaylineUndoCommand : public QUndoCommand {

    public:
        SetShowLaylineUndoCommand(SituationModel* situation, QUndoCommand *parent = 0);
        ~SetShowLaylineUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_SHOWLAYLINE; }

    private:
        SituationModel *m_situation;
};

class SetLaylineUndoCommand : public QUndoCommand {

    public:
        SetLaylineUndoCommand(SituationModel* situation, const int angle, QUndoCommand *parent = 0);
        ~SetLaylineUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_LAYLINE; }

    private:
        SituationModel *m_situation;
        int m_oldAngle;
        int m_newAngle;
};

class SetSituationSeriesUndoCommand : public QUndoCommand {

    public:
        SetSituationSeriesUndoCommand(SituationModel* situation, const int series, QUndoCommand *parent = 0);
        ~SetSituationSeriesUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_SITUATIONSERIES; }

    private:
        SituationModel *m_situation;
        int m_oldSeries;
        int m_newSeries;
};

class SetAbstractUndoCommand : public QUndoCommand {

    public:
        SetAbstractUndoCommand(SituationModel* situation, QString abstract, QUndoCommand *parent = 0);
        ~SetAbstractUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_ABSTRACT; }

    private:
        SituationModel *m_situation;
        QString m_oldAbstract;
        QString m_newAbstract;
};

class SetDescriptionUndoCommand : public QUndoCommand {

    public:
        SetDescriptionUndoCommand(SituationModel* situation, QString description, QUndoCommand *parent = 0);
        ~SetDescriptionUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_DESCRIPTION; }

    private:
        SituationModel *m_situation;
        QString m_oldDescription;
        QString m_newDescription;
};

class AddTrackUndoCommand : public QUndoCommand {

    public:
        AddTrackUndoCommand(SituationModel* situation, QUndoCommand *parent = 0);
        ~AddTrackUndoCommand();
        void undo();
        void redo();

        TrackModel *track() {return m_track; }

    private:
        SituationModel *m_situation;
        TrackModel *m_track;
};

class DeleteTrackUndoCommand : public QUndoCommand {

    public:
        DeleteTrackUndoCommand(SituationModel* situation, TrackModel* track, QUndoCommand *parent = 0);
        ~DeleteTrackUndoCommand();
        void undo();
        void redo();
    private:
        SituationModel *m_situation;
        TrackModel *m_track;
};

class SetSeriesUndoCommand : public QUndoCommand {

    public:
        SetSeriesUndoCommand(TrackModel* track, const Boats::Series series, QUndoCommand *parent = 0);
        ~SetSeriesUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_SERIES; }

    private:
        TrackModel *m_track;
        Boats::Series m_oldSeries;
        Boats::Series m_newSeries;
};

class SetColorUndoCommand : public QUndoCommand {

    public:
        SetColorUndoCommand(TrackModel* track, const QColor color, QUndoCommand *parent = 0);
        ~SetColorUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_COLOR; }

    private:
        TrackModel *m_track;
        QColor m_oldColor;
        QColor m_newColor;
};

class SetShowPathUndoCommand : public QUndoCommand {

    public:
        SetShowPathUndoCommand(TrackModel* track, QUndoCommand *parent = 0);
        ~SetShowPathUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_SHOWPATH; }

    private:
        TrackModel *m_track;
};

class MoveModelUndoCommand : public QUndoCommand {

    public:
        MoveModelUndoCommand(QList<PositionModel*> &modelList, const QPointF &deltaPosition, QUndoCommand *parent = 0);
        ~MoveModelUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return MOVE_MODEL; }
    private:
        QList<PositionModel*> m_modelList;
        QPointF m_deltaPosition;
};

class AddBoatUndoCommand : public QUndoCommand {

    public:
        AddBoatUndoCommand(TrackModel* track, QPointF& position, qreal heading, QUndoCommand *parent = 0);
        ~AddBoatUndoCommand();
        void undo();
        void redo();

        BoatModel *boat() {return m_boat; }

    private:
        TrackModel *m_track;
        BoatModel *m_boat;
};

class HeadingBoatUndoCommand : public QUndoCommand {

    public:
        HeadingBoatUndoCommand(QList<BoatModel*> &boatList, const qreal &heading, QUndoCommand *parent = 0);
        ~HeadingBoatUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return HEADING_BOAT; }
    private:
        QList<BoatModel*> m_boatList;
        QList<qreal> m_headingList;
        qreal m_heading;
};

class OverlapBoatUndoCommand : public QUndoCommand {

    public:
        OverlapBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, Boats::Overlaps overlaps, QUndoCommand *parent = 0);
        ~OverlapBoatUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return OVERLAP_BOAT; }
    private:
        SituationModel *m_situation;
        QList<BoatModel*> m_boatList;
        Boats::Overlaps m_overlaps;
};

class FlagBoatUndoCommand : public QUndoCommand {

    public:
        FlagBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, Boats::Flag flag, QUndoCommand *parent = 0);
        ~FlagBoatUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return FLAG_BOAT; }
    private:
        SituationModel *m_situation;
        QList<BoatModel*> m_boatList;
        QList<Boats::Flag> m_flagList;
        Boats::Flag m_flag;
};

class TrimBoatUndoCommand : public QUndoCommand {

    public:
        TrimBoatUndoCommand(QList<BoatModel*> &boatList, const qreal &trim, QUndoCommand *parent = 0);
        ~TrimBoatUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return TRIM_BOAT; }
    private:
        QList<BoatModel*> m_boatList;
        QList<qreal> m_trimList;
        qreal m_trim;
};

class SpinBoatUndoCommand : public QUndoCommand {

    public:
        SpinBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, bool spin, QUndoCommand *parent = 0);
        ~SpinBoatUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SPIN_BOAT; }
    private:
        SituationModel *m_situation;
        QList<BoatModel*> m_boatList;
        QList<bool> m_spinList;
        bool m_spin;
};

class SetTextUndoCommand : public QUndoCommand {

    public:
        SetTextUndoCommand(PositionModel *model, QString text, QUndoCommand *parent = 0);
        ~SetTextUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_TEXT; }

    private:
        PositionModel *m_model;
        QString m_oldText;
        QString m_newText;
};

class MoveTextUndoCommand : public QUndoCommand {

    public:
        MoveTextUndoCommand(PositionModel *model, const QPointF &deltaPosition, QUndoCommand *parent = 0);
        ~MoveTextUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return MOVE_TEXT; }
    private:
        PositionModel *m_model;
        QPointF m_deltaPosition;
};

class DeleteBoatUndoCommand : public QUndoCommand {

    public:
        DeleteBoatUndoCommand(TrackModel* track, BoatModel* boat, QUndoCommand *parent = 0);
        ~DeleteBoatUndoCommand();
        void undo();
        void redo();
    private:
        TrackModel *m_track;
        BoatModel *m_boat;
        int m_order;
};

class AddMarkUndoCommand : public QUndoCommand {

    public:
        AddMarkUndoCommand(SituationModel* situation, QPointF& position, QUndoCommand *parent = 0);
        ~AddMarkUndoCommand();
        void undo();
        void redo();

        MarkModel *mark() {return m_mark; }

    private:
        SituationModel *m_situation;
        MarkModel *m_mark;
};

class ZoneMarkUndoCommand : public QUndoCommand {

    public:
        ZoneMarkUndoCommand(SituationModel* situation, const QList<MarkModel*> &markList, QUndoCommand *parent = 0);
        ~ZoneMarkUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return ZONE_MARK; }

    private:
        SituationModel *m_situation;
        QList<MarkModel*> m_markList;
};

class LengthMarkUndoCommand : public QUndoCommand {

    public:
        LengthMarkUndoCommand(SituationModel* situation, const int length, QUndoCommand *parent = 0);
        ~LengthMarkUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return LENGTH_MARK; }

    private:
        SituationModel *m_situation;
        int m_oldLength;
        int m_newLength;
};

class DeleteMarkUndoCommand : public QUndoCommand {

    public:
        DeleteMarkUndoCommand(SituationModel* situation, MarkModel* mark, QUndoCommand *parent = 0);
        ~DeleteMarkUndoCommand();
        void undo();
        void redo();
    private:
        SituationModel *m_situation;
        MarkModel *m_mark;
        int m_order;
};

class AddPolyLineUndoCommand : public QUndoCommand {

    public:
        AddPolyLineUndoCommand(SituationModel* situation, QUndoCommand *parent = 0);
        ~AddPolyLineUndoCommand();
        void undo();
        void redo();

        PolyLineModel *polyLine() {return m_polyLine; }

    private:
        SituationModel *m_situation;
        PolyLineModel *m_polyLine;
};

class DeletePolyLineUndoCommand : public QUndoCommand {

    public:
        DeletePolyLineUndoCommand(SituationModel* situation, PolyLineModel* polyLine, QUndoCommand *parent = 0);
        ~DeletePolyLineUndoCommand();
        void undo();
        void redo();
    private:
        SituationModel *m_situation;
        PolyLineModel *m_polyLine;
};

class AddPointUndoCommand : public QUndoCommand {

    public:
        AddPointUndoCommand(PolyLineModel* polyLine, QPointF& position, QUndoCommand *parent = 0);
        ~AddPointUndoCommand();
        void undo();
        void redo();

        PointModel *point() {return m_point; }

    private:
        PolyLineModel *m_polyLine;
        PointModel *m_point;
};

class DeletePointUndoCommand : public QUndoCommand {

    public:
        DeletePointUndoCommand(PolyLineModel* polyLine, PointModel* point, QUndoCommand *parent = 0);
        ~DeletePointUndoCommand();
        void undo();
        void redo();
    private:
        PolyLineModel *m_polyLine;
        PointModel *m_point;
        int m_order;
};

#endif
