//
// C++ Interface: undocommands
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
#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H

#include "boats.h"

#include <QUndoCommand>
#include <QPointF>

class SituationModel;
class TrackModel;
class PositionModel;
class BoatModel;
class MarkModel;
class PolyLineModel;
class PointModel;
class WindModel;

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
    SET_FOLLOWTRACK,
    SET_LOOKAT,
    SET_SHOWWIND,
    SET_WIND,
    MOVE_MODEL,
    SET_LAYLINES,
    ROTATE_BOATS,
    OVERLAP_BOAT,
    FLAG_BOAT,
    TRIM_BOAT,
    TRIM_JIB,
    TRIM_SPIN,
    SPIN_BOAT,
    HIDE_BOAT,
    ACCELERATE_BOAT,
    SET_TEXT,
    MOVE_TEXT,
    ZONE_MARK,
    COLOR_MARK,
    LENGTH_MARK,
    LABEL_MARK
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

class SetFollowTrackUndoCommand : public QUndoCommand {

    public:
        SetFollowTrackUndoCommand(SituationModel* situation, TrackModel* track, QUndoCommand *parent = 0);
        ~SetFollowTrackUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_FOLLOWTRACK; }

    private:
        SituationModel *m_situation;
        TrackModel *m_track;
        QList<bool> m_followTrackList;
};

class SetLookAtUndoCommand : public QUndoCommand {

    public:
        SetLookAtUndoCommand(SituationModel* situation, int lookDirection, int tilt, QUndoCommand *parent = 0);
        ~SetLookAtUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_LOOKAT; }

    private:
        SituationModel *m_situation;
        int m_oldLookDirection;
        int m_newLookDirection;
        int m_oldTilt;
        int m_newTilt;
};

class SetShowWindUndoCommand : public QUndoCommand {

    public:
        SetShowWindUndoCommand(WindModel* wind, QUndoCommand *parent = 0);
        ~SetShowWindUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_SHOWWIND; }

    private:
        WindModel *m_wind;
};


class AddWindUndoCommand : public QUndoCommand {

    public:
        AddWindUndoCommand(WindModel* wind, qreal heading, QUndoCommand *parent = 0);
        ~AddWindUndoCommand();
        void undo();
        void redo();

    private:
        WindModel *m_wind;
        qreal m_heading;
};

class SetWindUndoCommand : public QUndoCommand {

    public:
        SetWindUndoCommand(WindModel* wind, int index, qreal direction, QUndoCommand *parent = 0);
        ~SetWindUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_WIND; }

    private:
        WindModel *m_wind;
        int m_index;
        qreal m_oldDirection;
        qreal m_newDirection;
};

class DeleteWindUndoCommand : public QUndoCommand {

    public:
        DeleteWindUndoCommand(WindModel* wind, int index, QUndoCommand *parent = 0);
        ~DeleteWindUndoCommand();
        void undo();
        void redo();

    private:
        WindModel *m_wind;
        int m_index;
        qreal m_heading;
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

class SetLaylinesUndoCommand : public QUndoCommand {

    public:
        SetLaylinesUndoCommand(QList<PositionModel*> &modelList, bool laylines, QUndoCommand *parent = 0);
        ~SetLaylinesUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return SET_LAYLINES; }

    private:
        QList<PositionModel*> m_modelList;
        QList<bool> m_laylinesList;
        bool m_laylines;
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

class RotateModelsUndoCommand : public QUndoCommand {

    public:
        RotateModelsUndoCommand(QList<PositionModel*> &modelList, const qreal &angle, QUndoCommand *parent = 0);
        ~RotateModelsUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return ROTATE_BOATS; }
    private:
        QList<PositionModel*> m_modelList;
        QList<qreal> m_headingList;
        qreal m_angle;
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
        QList<qreal> m_jibTrimList;
        qreal m_trim;
};

class TrimJibUndoCommand : public QUndoCommand {

    public:
        TrimJibUndoCommand(QList<BoatModel*> &boatList, const qreal &trim, QUndoCommand *parent = 0);
        ~TrimJibUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return TRIM_JIB; }
    private:
        QList<BoatModel*> m_boatList;
        QList<qreal> m_trimList;
        qreal m_trim;
};

class TrimSpinUndoCommand : public QUndoCommand {

    public:
        TrimSpinUndoCommand(QList<BoatModel*> &boatList, const qreal &trim, QUndoCommand *parent = 0);
        ~TrimSpinUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return TRIM_SPIN; }
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

class HiddenBoatUndoCommand : public QUndoCommand {

    public:
        HiddenBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, bool hidden, QUndoCommand *parent = 0);
        ~HiddenBoatUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return HIDE_BOAT; }
    private:
        SituationModel *m_situation;
        QList<BoatModel*> m_boatList;
        QList<bool> m_hiddenList;
        bool m_hidden;
};

class AccelerateBoatUndoCommand : public QUndoCommand {

    public:
        AccelerateBoatUndoCommand(SituationModel* situation, QList<BoatModel*> &boatList, Boats::Acceleration acceleration, QUndoCommand *parent = 0);
        ~AccelerateBoatUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return ACCELERATE_BOAT; }
    private:
        SituationModel *m_situation;
        QList<BoatModel*> m_boatList;
        QList<Boats::Acceleration> m_accelerationList;
        Boats::Acceleration m_acceleration;
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

class ColorMarkUndoCommand : public QUndoCommand {

    public:
        ColorMarkUndoCommand(SituationModel* situation, const QList<MarkModel*> &markList, const QColor &color, QUndoCommand *parent = 0);
        ~ColorMarkUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return COLOR_MARK; }

    private:
        SituationModel *m_situation;
        QList<MarkModel*> m_markList;
        QList<QColor> m_oldColors;
        QColor m_newColor;
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

class ToggleMarkSideUndoCommand : public QUndoCommand {

    public:
        ToggleMarkSideUndoCommand(const QList<MarkModel *> &markList, QUndoCommand *parent = 0);
        ~ToggleMarkSideUndoCommand();
        void undo();
        void redo();

    private:
        QList<MarkModel*> m_markList;
};

class ToggleMarkArrowUndoCommand : public QUndoCommand {

    public:
        ToggleMarkArrowUndoCommand(const QList<MarkModel *> &markList, QUndoCommand *parent = 0);
        ~ToggleMarkArrowUndoCommand();
        void undo();
        void redo();

    private:
        QList<MarkModel*> m_markList;
};

class ToggleMarkLabelUndoCommand : public QUndoCommand {

    public:
        ToggleMarkLabelUndoCommand(const QList<MarkModel*> &markList, QUndoCommand *parent = 0);
        ~ToggleMarkLabelUndoCommand();
        void undo();
        void redo();

    private:
        QList<MarkModel*> m_markList;
};

class SetMarkLabelUndoCommand : public QUndoCommand {

    public:
        SetMarkLabelUndoCommand(MarkModel* mark, QString text, QUndoCommand *parent = 0);
        ~SetMarkLabelUndoCommand();
        void undo();
        void redo();
        bool mergeWith(const QUndoCommand *command);
        int id() const { return LABEL_MARK; }

    private:
        MarkModel* m_mark;
        QString m_oldText;
        QString m_newText;
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
