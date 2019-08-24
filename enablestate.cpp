//
// C++ Implementation: enablestate
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
#include "enablestate.h"

#include "commontypes.h"

#include <QStateMachine>
#include <QAbstractTransition>
#include <QAbstractState>

#include <iostream>

extern int debugLevel;

EnableState::EnableState(QState *parent) :
    QState(parent),
    m_active(false),
    m_enabled(false)
{
}

bool EnableState::isActive() {
    return m_active;
}

bool EnableState::isEnabled() {
    return m_enabled;
}

bool EnableState::isToggable() {
    return m_toggable;
}

void EnableState::onEntry(QEvent *event) {
    Q_UNUSED(event);
    if(!m_active) {
        m_active = true;
        if (debugLevel & 1 << STATE) std::cout << "State Entry " << this->objectName().toStdString() << std::endl;
        emit activeChanged(m_active);
        foreach (QAbstractTransition* transition, transitions()) {
            EnableState *state = static_cast<EnableState*> (transition->targetState());
            state->setEnabled(true);
        }
        if (m_toggable) {
            setEnabled(true);
        }
    }
}

void EnableState::onExit(QEvent *event) {
    Q_UNUSED(event);
    if(m_active) {
        m_active = false;
        if (debugLevel & 1 << STATE) std::cout << "State Exit " << this->objectName().toStdString() << std::endl;
        emit activeChanged(m_active);
        foreach (QAbstractTransition* transition, transitions()) {
            EnableState *state = static_cast<EnableState*> (transition->targetState());
            state->setEnabled(false);
        }
        if (m_toggable) {
            setEnabled(false);
        }
    }
}

void EnableState::setEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged(m_enabled);
    }
}

void EnableState::setToggable(bool toggable) {
    if (m_toggable != toggable) {
        m_toggable = toggable;
        emit toggableChanged(m_toggable);
    }
}
