//
// C++ Interface: state
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
#ifndef ENABLESTATE_H
#define ENABLESTATE_H

#include <QState>

class EnableState : public QState
{
    Q_OBJECT
    Q_PROPERTY(bool isActive READ isActive NOTIFY activeChanged)
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool isToggable READ isToggable WRITE setToggable NOTIFY toggableChanged)

public:
    explicit EnableState(QState *parent = 0);

    bool isActive();
    bool isEnabled();
    bool isToggable();
    void setEnabled(bool enabled);
    void setToggable(bool toggable);

signals:
    void activeChanged(bool);
    void enabledChanged(bool);
    void toggableChanged(bool);

protected:
    void onEntry(QEvent *event);
    void onExit(QEvent *event);

private:
    bool m_active;
    bool m_enabled;
    bool m_toggable;
};

#endif // ENABLESTATE_H
