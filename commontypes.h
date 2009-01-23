//
// C++ Interface: commontypes
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#define VERSION "200811"

enum DebugTraces {
    MODEL,
    VIEW,
    COMMAND,
    ANIMATION,
    DELEGATE
};

typedef enum {
    TRACK_COLOR,
    TRACK_SERIES
} TrackTableItem;

#endif
