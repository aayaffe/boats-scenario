//
// C++ Interface: statemachine
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
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QStateMachine>

#include "enablestate.h"

class StateMachine : public QStateMachine
{
    Q_OBJECT
    Q_PROPERTY(EnableState* parallelEditionState READ parallelEditionState CONSTANT)
    Q_PROPERTY(EnableState* editingState READ editingState CONSTANT)
    Q_PROPERTY(EnableState* mouseState READ mouseState CONSTANT)
    Q_PROPERTY(EnableState* animationState READ animationState CONSTANT)

    Q_PROPERTY(EnableState* noStateState READ noStateState CONSTANT)
    Q_PROPERTY(EnableState* createState READ createState CONSTANT)
    Q_PROPERTY(EnableState* createTrackState READ createTrackState CONSTANT)
    Q_PROPERTY(EnableState* createBoatState READ createBoatState CONSTANT)
    Q_PROPERTY(EnableState* createMarkState READ createMarkState CONSTANT)
    Q_PROPERTY(EnableState* createLineState READ createLineState CONSTANT)
    Q_PROPERTY(EnableState* createPointState READ createPointState CONSTANT)

    Q_PROPERTY(EnableState* selectState READ selectState CONSTANT)
    Q_PROPERTY(EnableState* moveState READ moveState CONSTANT)
    Q_PROPERTY(EnableState* rotateState READ rotateState CONSTANT)

    Q_PROPERTY(EnableState* stopState READ stopState CONSTANT)
    Q_PROPERTY(EnableState* playState READ playState CONSTANT)
    Q_PROPERTY(EnableState* pauseState READ pauseState CONSTANT)

public:
    explicit StateMachine(QObject *parent = 0);

    EnableState *parallelEditionState();
    EnableState *editingState();
    EnableState *mouseState();
    EnableState *animationState();

    EnableState *noStateState();
    EnableState *createState();
    EnableState *createTrackState();
    EnableState *createBoatState();
    EnableState *createMarkState();
    EnableState *createLineState();
    EnableState *createPointState();
    EnableState *selectState();
    EnableState *moveState();
    EnableState *rotateState();
    EnableState *stopState();
    EnableState *playState();
    EnableState *pauseState();

signals:
    void edit();
    void animate();
    void noState();
    void createTrack();
    void createBoat();
    void createMark();
    void createLine();
    void createPoint();
    void move();
    void lmbMove();
    void rmbMove();
    void stop();
    void play();
    void pause();
    void lmbclick();
    void rmbclick();

public slots:
    void setCreateMouseTransitions();
    void unsetCreateMouseTransitions();
    void setNoStateMouseTransitions();
    void unsetNoStateMouseTransitions();

private:
    EnableState* m_parallelEditionState;
    EnableState* m_editingState;
    EnableState* m_mouseState;
    EnableState* m_animationState;

    EnableState* m_noStateState;
    EnableState* m_createState;
    EnableState* m_createTrackState;
    EnableState* m_createBoatState;
    EnableState* m_createMarkState;
    EnableState* m_createLineState;
    EnableState* m_createPointState;

    EnableState* m_selectState;
    EnableState* m_moveState;
    EnableState* m_rotateState;

    EnableState* m_stopState;
    EnableState* m_playState;
    EnableState* m_pauseState;
};

#endif // STATEMACHINE_H
