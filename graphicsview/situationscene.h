//
// C++ Interface: situationscene
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
#ifndef SITUATIONSCENE_H
#define SITUATIONSCENE_H

#include <QtGui>

class SituationModel;
class TrackModel;
class PositionModel;
class BoatModel;
class MarkModel;
class PolyLineModel;
class PointModel;
class TrackAnimation;
class ScenarioAnimation;

/**
    \enum SceneState

    This enum defines the different modal states of the edition. When
    the user wants to input data to the scenario, he sets the current
    edition mode.

    \value NO_STATE No particular mode is set. In this mode item selection
    and movement is allowed.

    \value CREATE_TRACK The Create Track mode will create a new TrackModel
    object in the scenario. This mode is then overriden to CREATE_BOAT as
    a natural action is to append boat positions.

    \value CREATE_BOAT The Create Boat will append a BoatModel to the
    track of the lastly selected boat object.

    \value CREATE_MARK The Create Mark will create a new MarkModel to
    the scenario.

    \value ANIMATE The Animation will be prepared, and no further input
    will be allowed in this mode.

*/

typedef enum {
    NO_STATE,
    CREATE_TRACK,
    CREATE_BOAT,
    CREATE_MARK,
    CREATE_LINE,
    CREATE_POINT,
    ANIMATE
} SceneState;

enum {
    BOAT_TYPE = QGraphicsItem::UserType + 1,
    MARK_TYPE,
    POINT_TYPE,
    ARROW_TYPE
};

/**
    \class SituationScene

    \brief The Scene for a scenario

    The class represents the Scene for a Scenario, according to the
    Graphics View Framework.

    It inherits QGraphicsScene and manages user input.

    The class handles a modal input status to trigger various actions
    depending on the mode.

    Action on Objects is handled by overloading the event methods:
    keyPressEvent(), mousePressEvent(), mouseReleaseEvent() and
    mouseMoveEvent().

    Specialised methods handle the possible events and generally use
    Undo Framework classes to modify the ScenarioModel.

    QGraphicsItem Objects selection is handled through setSelectedModels()
    slot, and updates the various Lists needed to determine matching Model
    objects.

    The class provides slots to react to SituationModel changes and update
    drawing accordingly.

    The class prepares the drawing of animation as well, through
    setAnimation() and unSetAnimation() methods.

    \sa QGraphicsScene, SituationModel, BoatGraphicsItem, MarkGraphicsItem
*/

class SituationScene : public QGraphicsScene {
        Q_OBJECT
    public:
        SituationScene(SituationModel* situation);
        ~SituationScene() {}

        void setState(const SceneState& theValue, bool commit = false);
        void setModelPressed(PositionModel *theValue) {m_modelPressed = theValue; }
        SceneState state() const { return m_state; }
        QList< PositionModel * > selectedModels() const { return m_selectedModels; }
        QList< BoatModel * > selectedBoatModels() const { return m_selectedBoatModels; }
        QList< MarkModel * > selectedMarkModels() const { return m_selectedMarkModels; }
        QList< PointModel * > selectedPointModels() const { return m_selectedPointModels; }
        void setDefaultPopup(QMenu *theValue) { m_defaultPopup = theValue; }
        void setBoatPopup(QMenu *theValue) { m_boatPopup = theValue; }
        void setMarkPopup(QMenu *theValue) { m_markPopup = theValue; }
        void setPointPopup(QMenu *theValue) { m_pointPopup = theValue; }
        ScenarioAnimation *animation() const { return m_scenarioAnimation; }

    signals:
        void itemMoved(QList<BoatModel*> movedItems, const QPointF &movedFromPosition);
        void stateChanged(SceneState newState);
        void selectedModelsChanged();
        void centerChanged(QPointF position);
        void lookDirectionChanged( const qreal lookDirection);
        void tiltChanged( const qreal tilt);

    public slots:
        // Slot for selection mechanism
        void setSelectedModels();

        // Slots for SituationModel signals
        void addTrack(TrackModel *track);
        void addBoatItem(BoatModel *boat);
        void addMarkItem(MarkModel *mark);
        void addPolyLine(PolyLineModel *polyline);
        void addPoint(PointModel *point);
        void setLaylines(const int angle);
        void setWind(bool visible);

        // Slots for animation signals
        void setAnimation();
        void unSetAnimation();

    protected:
        // Overloaded methods for Events
        void keyPressEvent(QKeyEvent *event);
        void mousePressEvent(QGraphicsSceneMouseEvent *event);
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        void mouseClickEvent(QGraphicsSceneMouseEvent *event);
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    private:
        // Specialised Event methods
        void mouseSelectEvent(QGraphicsSceneMouseEvent *event);
        void createTrack(QPointF pos);
        void moveModel(QPointF pos);
        void headingModel(QPointF pos);
        void createBoat(QPointF pos);
        void createMark(QPointF pos);
        void createLine(QPointF pos);
        void createPoint(QPointF pos);

        SituationModel *m_situation;

        // Bookkeeping references to selected models
        /// \a m_selectedModels holds the list of selected PositionModel
        QList<PositionModel*> m_selectedModels;

        /// \a m_selectedBoatsModels holds the list of selected BoatModel
        QList<BoatModel*> m_selectedBoatModels;

        /// \a m_selectedMarkModels holds the list of selected MarkModel
        QList<MarkModel*> m_selectedMarkModels;

        /// \a m_selectedPointModels holds the list of selected PointModel
        QList<PointModel*> m_selectedPointModels;

        /// \a m_windAnimation holds the animation for wind
        QAnimationGroup *m_windAnimation;

        /// \a m_animationItems holds the list of TrackAnimation items
        /// created for animation mode
        QList<TrackAnimation*> m_animationItems;

        /// \a m_scenarioAnimation holds the general AnimationGroup
        /// manipulated during animation mode
        ScenarioAnimation* m_scenarioAnimation;

        /// \a m_modelPressed holds the BoatModel being pressed
        PositionModel* m_modelPressed;

        /// \a m_trackCreated holds the last selected TrackModel
        TrackModel *m_trackCreated;

        /// \a m_polyLineCreated holds the last selected PolyLineModel
        PolyLineModel *m_polyLineCreated;

        /// \a m_markCreated holds the last selected MarkModel
        MarkModel *m_markCreated;

        /// \a m_fromPosition holds the QPointF where mouse was pressed
        QPointF m_fromPosition;

        /// \a m_curPosition holds the QPointF where mouse was last seen
        QPointF m_curPosition;

        /// \a m_state holds the SceneState for the current scenario
        SceneState m_state;

        /// \a m_time holds the timer used for movement filtering
        QTime m_time;

        /// \a m_clickTime holds the timer used for click/press detection
        QTime m_clickTime;

        enum {
            SINGLE,
            DOUBLE,
            NONE
        } m_clickState;

        QMenu *m_defaultPopup;
        QMenu *m_boatPopup;
        QMenu *m_markPopup;
        QMenu *m_pointPopup;
};

#endif
