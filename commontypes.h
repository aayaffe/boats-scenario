//
// C++ Interface: commontypes
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2009 Thibaut GRIDEL
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
#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#define VERSION "200910"

enum DebugTraces {
    MODEL,
    VIEW,
    COMMAND,
    ANIMATION,
    DELEGATE,
    XML,
    EXPORT
};

typedef enum {
    TRACK_COLOR,
    TRACK_PATH,
    TRACK_SERIES
} TrackTableItem;

#endif
