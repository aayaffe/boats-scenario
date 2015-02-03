//
// C++ Implementation: statemachine
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2014 Thibaut GRIDEL
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
#include "statemachine.h"

StateMachine::StateMachine(QObject *parent) :
    QStateMachine(parent),
    m_parallelEditionState(new EnableState(this)),
    m_editingState(new EnableState(m_parallelEditionState)),
    m_mouseState(new EnableState(m_parallelEditionState)),
    m_selectionState(new EnableState(m_parallelEditionState)),
    m_animationState(new EnableState(this)),
    m_noStateState(new EnableState(m_editingState)),
    m_createState(new EnableState(m_editingState)),
    m_createTrackState(new EnableState(m_createState)),
    m_createBoatState(new EnableState(m_createState)),
    m_createMarkState(new EnableState(m_createState)),
    m_createLineState(new EnableState(m_createState)),
    m_createPointState(new EnableState(m_createState)),
    m_selectState(new EnableState(m_mouseState)),
    m_moveState(new EnableState(m_mouseState)),
    m_rotateState(new EnableState(m_mouseState)),
    m_noSelectionState(new EnableState(m_selectionState)),
    m_boatSelectionState(new EnableState(m_selectionState)),
    m_pointSelectionState(new EnableState(m_selectionState)),
    m_stopState(new EnableState(m_animationState)),
    m_playState(new EnableState(m_animationState)),
    m_pauseState(new EnableState(m_animationState))
{
    m_parallelEditionState->setObjectName("ParallelEdition");
    m_editingState->setObjectName("Editing");
    m_mouseState->setObjectName("Mouse");
    m_animationState->setObjectName("ANIMATE");
    m_noStateState->setObjectName("NO_STATE");
    m_createState->setObjectName("Create");
    m_createTrackState->setObjectName("CREATE_TRACK");
    m_createBoatState->setObjectName("CREATE_BOAT");
    m_createMarkState->setObjectName("CREATE_MARK");
    m_createLineState->setObjectName("CREATE_LINE");
    m_createPointState->setObjectName("CREATE_POINT");
    m_selectState->setObjectName("Select");
    m_moveState->setObjectName("Move");
    m_rotateState->setObjectName("Rotate");
    m_noSelectionState->setObjectName("No Selection");
    m_boatSelectionState->setObjectName("Boat Selection");
    m_pointSelectionState->setObjectName("Point Selection");
    m_stopState->setObjectName("STOP");
    m_playState->setObjectName("PLAY");
    m_pauseState->setObjectName("PAUSE");

    setInitialState(m_parallelEditionState);
    m_parallelEditionState->setChildMode(QStateMachine::ParallelStates);
    m_editingState->setInitialState(m_noStateState);
    m_editingState->addTransition(this, SIGNAL(animate()), m_animationState);
    m_mouseState->setInitialState(m_selectState);
    m_selectionState->setInitialState(m_noSelectionState);

    m_animationState->setInitialState(m_stopState);
    m_animationState->setToggable(true);
    m_animationState->addTransition(this, SIGNAL(edit()), m_editingState);
    m_animationState->addTransition(this, SIGNAL(noState()), m_editingState);

    m_noStateState->addTransition(this, SIGNAL(createTrack()), m_createTrackState);
    m_noStateState->addTransition(this, SIGNAL(createBoat()), m_createBoatState);
    m_noStateState->addTransition(this, SIGNAL(createMark()), m_createMarkState);
    m_noStateState->addTransition(this, SIGNAL(createLine()), m_createLineState);
    m_noStateState->addTransition(this, SIGNAL(createPoint()), m_createPointState);

    m_createTrackState->setToggable(true);
    m_createTrackState->addTransition(this, SIGNAL(noState()), m_noStateState);
    m_createTrackState->addTransition(this, SIGNAL(lmbclick()), m_createBoatState);
    m_createTrackState->addTransition(this, SIGNAL(createTrack()), m_noStateState);
    m_createTrackState->addTransition(this, SIGNAL(createBoat()), m_createBoatState);
    m_createTrackState->addTransition(this, SIGNAL(createMark()), m_createMarkState);
    m_createTrackState->addTransition(this, SIGNAL(createLine()), m_createLineState);

    m_createBoatState->setToggable(true);
    m_createBoatState->addTransition(this, SIGNAL(noState()), m_noStateState);
    m_createBoatState->addTransition(this, SIGNAL(lmbclick()), m_createBoatState);
    m_createBoatState->addTransition(this, SIGNAL(createTrack()), m_createTrackState);
    m_createBoatState->addTransition(this, SIGNAL(createBoat()), m_noStateState);
    m_createBoatState->addTransition(this, SIGNAL(createMark()), m_createMarkState);
    m_createBoatState->addTransition(this, SIGNAL(createLine()), m_createLineState);

    m_createMarkState->setToggable(true);
    m_createMarkState->addTransition(this, SIGNAL(noState()), m_noStateState);
    m_createMarkState->addTransition(this, SIGNAL(lmbclick()), m_createMarkState);
    m_createMarkState->addTransition(this, SIGNAL(createTrack()), m_createTrackState);
    m_createMarkState->addTransition(this, SIGNAL(createMark()), m_noStateState);
    m_createMarkState->addTransition(this, SIGNAL(createLine()), m_createLineState);

    m_createLineState->setToggable(true);
    m_createLineState->addTransition(this, SIGNAL(noState()), m_noStateState);
    m_createLineState->addTransition(this, SIGNAL(lmbclick()), m_createPointState);
    m_createLineState->addTransition(this, SIGNAL(createTrack()), m_createTrackState);
    m_createLineState->addTransition(this, SIGNAL(createMark()), m_createMarkState);
    m_createLineState->addTransition(this, SIGNAL(createLine()), m_noStateState);
    m_createLineState->addTransition(this, SIGNAL(createPoint()), m_createPointState);

    m_createPointState->setToggable(true);
    m_createPointState->addTransition(this, SIGNAL(noState()), m_noStateState);
    m_createPointState->addTransition(this, SIGNAL(lmbclick()), m_createPointState);
    m_createPointState->addTransition(this, SIGNAL(createTrack()), m_createTrackState);
    m_createPointState->addTransition(this, SIGNAL(createMark()), m_createMarkState);
    m_createPointState->addTransition(this, SIGNAL(createLine()), m_createLineState);
    m_createPointState->addTransition(this, SIGNAL(createPoint()), m_noStateState);

    m_noSelectionState->addTransition(this, SIGNAL(selectBoat()), m_boatSelectionState);
    m_noSelectionState->addTransition(this, SIGNAL(selectPoint()), m_pointSelectionState);

    m_boatSelectionState->addTransition(this, SIGNAL(clearSelection()), m_noSelectionState);

    m_pointSelectionState->addTransition(this, SIGNAL(clearSelection()), m_noSelectionState);

    m_stopState->addTransition(this, SIGNAL(play()), m_playState);

    m_playState->addTransition(this, SIGNAL(stop()), m_stopState);
    m_playState->addTransition(this, SIGNAL(pause()), m_pauseState);

    m_pauseState->setToggable(true);
    m_pauseState->addTransition(this, SIGNAL(stop()), m_stopState);
    m_pauseState->addTransition(this, SIGNAL(pause()), m_playState);
    m_pauseState->addTransition(this, SIGNAL(play()), m_playState);

    connect(m_createState, SIGNAL(entered()),
            this, SLOT(setCreateMouseTransitions()));
    connect(m_createState, SIGNAL(exited()),
            this, SLOT(unsetCreateMouseTransitions()));
    connect(m_noStateState, SIGNAL(entered()),
            this, SLOT(setNoStateMouseTransitions()));
    connect(m_noStateState, SIGNAL(exited()),
            this, SLOT(unsetNoStateMouseTransitions()));
}

void StateMachine::setCreateMouseTransitions() {
    m_selectState->addTransition(this, SIGNAL(move()), m_moveState);
    m_selectState->addTransition(this, SIGNAL(rmbMove()), m_rotateState);

    m_moveState->addTransition(this, SIGNAL(lmbclick()), m_selectState);
    m_moveState->addTransition(this, SIGNAL(move()), m_moveState);
    m_moveState->addTransition(this, SIGNAL(rmbMove()), m_rotateState);

    m_rotateState->addTransition(this, SIGNAL(move()), m_moveState);
    m_rotateState->addTransition(this, SIGNAL(rmbMove()), m_rotateState);
}

void StateMachine::unsetCreateMouseTransitions() {
    foreach(QAbstractTransition *transition, m_selectState->transitions()) {
        m_selectState->removeTransition(transition);
    }

    foreach(QAbstractTransition *transition, m_moveState->transitions()) {
        m_moveState->removeTransition(transition);
    }
    m_moveState->addTransition(m_noStateState, SIGNAL(entered()), m_selectState);

    foreach(QAbstractTransition *transition, m_rotateState->transitions()) {
        m_rotateState->removeTransition(transition);
    }
    m_rotateState->addTransition(m_noStateState, SIGNAL(entered()), m_selectState);
}

void StateMachine::setNoStateMouseTransitions() {
    m_selectState->addTransition(this, SIGNAL(lmbMove()), m_moveState);
    m_selectState->addTransition(this, SIGNAL(rmbMove()), m_rotateState);

    m_moveState->addTransition(this, SIGNAL(lmbclick()), m_selectState);
    m_moveState->addTransition(this, SIGNAL(lmbMove()), m_moveState);

    m_rotateState->addTransition(this, SIGNAL(rmbclick()), m_selectState);
    m_rotateState->addTransition(this, SIGNAL(rmbMove()), m_rotateState);
}

void StateMachine::unsetNoStateMouseTransitions() {
    foreach(QAbstractTransition *transition, m_selectState->transitions()) {
        m_selectState->removeTransition(transition);
    }

    foreach(QAbstractTransition *transition, m_moveState->transitions()) {
        m_moveState->removeTransition(transition);
    }
    m_moveState->addTransition(m_createState, SIGNAL(entered()), m_selectState);

    foreach(QAbstractTransition *transition, m_rotateState->transitions()) {
        m_rotateState->removeTransition(transition);
    }
    m_rotateState->addTransition(m_createState, SIGNAL(entered()), m_selectState);
}

EnableState *StateMachine::parallelEditionState() {
    return m_parallelEditionState;
}

EnableState *StateMachine::editingState() {
    return m_editingState;
}

EnableState *StateMachine::mouseState() {
    return m_mouseState;
}

EnableState *StateMachine::selectionState() {
    return m_selectionState;
}

EnableState *StateMachine::animationState() {
    return m_animationState;
}

EnableState *StateMachine::noStateState() {
    return m_noStateState;
}

EnableState *StateMachine::createState() {
    return m_createState;
}

EnableState *StateMachine::createTrackState() {
    return m_createTrackState;
}

EnableState *StateMachine::createBoatState() {
    return m_createBoatState;
}

EnableState *StateMachine::createMarkState() {
    return m_createMarkState;
}

EnableState *StateMachine::createLineState() {
    return m_createLineState;
}

EnableState *StateMachine::createPointState() {
    return m_createPointState;
}

EnableState *StateMachine::selectState() {
    return m_selectState;
}

EnableState *StateMachine::moveState() {
    return m_moveState;
}


EnableState *StateMachine::rotateState() {
    return m_rotateState;
}

EnableState *StateMachine::noSelectionState() {
    return m_noSelectionState;
}

EnableState *StateMachine::boatSelectionState() {
    return m_boatSelectionState;
}

EnableState *StateMachine::pointSelectionState() {
    return m_pointSelectionState;
}

EnableState *StateMachine::stopState() {
    return m_stopState;
}

EnableState *StateMachine::playState() {
    return m_playState;
}

EnableState *StateMachine::pauseState() {
    return m_pauseState;
}
