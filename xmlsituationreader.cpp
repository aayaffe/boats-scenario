//
// C++ Implementation: XmlSituationReader
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
#include <iostream>

#include "xmlsituationreader.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"
#include "windmodel.h"

#include "undocommands.h"

XmlSituationReader::XmlSituationReader(SituationModel *situation)
        : m_situation(situation) {
}

XmlSituationReader::~XmlSituationReader() {}

bool XmlSituationReader::read(QIODevice *device) {
    setDevice(device);

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (name() == "xmlscenario" && attributes().value("version") == "1.0")
                readSituation();
            else
                raiseError(QObject::tr("The file is not an xmlscenario version 1.0 file."));
        }
    }
    return !error();
}

QString XmlSituationReader::readUnknownElement() {
    QString elem;
    elem.append("<").append(name()).append(">");
    while (!atEnd()) {
        readNext();
        if (isCharacters())
            elem.append(text());
        else if (isStartElement())
            elem.append(readUnknownElement());

        else if (isEndElement()) {
            elem.append("</").append(name()).append(">");
            break;
        }
    }
    std::cout << "unread data " << elem.toStdString() << std::endl;
    return elem;
}

void XmlSituationReader::readSituation() {
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;
        if (isStartElement()) {
            if (name() == "title") {
                m_situation->changeTitle(readElementText());

            } else if (name() == "rules") {
                m_situation->changeRules(readElementText());

            } else if (name() == "abstract") {
                m_situation->changeAbstract(readElementText());

            } else if (name() == "description") {
                m_situation->changeDescription(readElementText());

            } else if (name() == "series") {
                m_situation->changeSeries(series(readElementText()));

            } else if (name() == "showlayline") {
                if (readElementText() == "0") {
                    m_situation->toggleShowLayline(false);
                }

            } else if (name() == "layline") {
                m_situation->changeLaylineAngle(readElementText().toInt());

            } else if (name() == "length") {
                m_situation->setSituationLength(readElementText().toInt());

            } else if (name() == "look_direction") {
                m_situation->setLookDirection(readElementText().toFloat());

            } else if (name() == "tilt") {
                m_situation->setTilt(readElementText().toFloat());

            } else if (name() == "mark") {
                readMark(m_situation);

            } else if (name() == "track") {
                readTrack(m_situation);

            } else if (name() == "polyline") {
                readPolyLine(m_situation);

            } else if (name() == "wind") {
                readWind(m_situation);

            } else {
                m_situation->appendDiscardedXml(readUnknownElement());
            }
        }
    }
}

void XmlSituationReader::readTrack(SituationModel *situation) {
    AddTrackUndoCommand *command = new AddTrackUndoCommand(situation);
    situation->undoStack()->push(command);
    TrackModel *track = command->track();
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;
        if (isStartElement()) {
            if (name() == "color")
                track->setColor(QColor(readElementText()));
            else if (name() == "series")
                track->setSeries(series(readElementText()));
            else if (name() == "path")
                track->setShowPath(readElementText() == "1");
            else if (name() == "follow_track")
                track->setFollowTrack(readElementText() == "1");
            else if (name() == "boat")
                readBoat(situation, track);
            else
                track->appendDiscardedXml(readUnknownElement());
        }
    }
}

void XmlSituationReader::readBoat(SituationModel *situation, TrackModel *track) {
    QPointF pos;
    qreal heading = 0;
    qreal trim = 0;
    qreal jibTrim = 0;
    bool spin = false;
    qreal spinTrim = 0;

    Boats::Overlaps overlap = Boats::none;
    Boats::Flag flag = Boats::noFlag;
    bool hidden = false;
    Boats::Acceleration acceleration = Boats::constant;
    QPointF textPos(10,10);
    QString text;
    bool laylines = 0;
    QStringList discarded;
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;
        if (isStartElement()) {
            if (name() == "x")
                pos.setX(readElementText().toFloat());
            else if (name() == "y")
                pos.setY(readElementText().toFloat());
            else if (name() == "heading")
                heading = readElementText().toFloat();
            else if (name() == "trim")
                trim = readElementText().toFloat();
            else if (name() == "jibtrim")
                jibTrim = readElementText().toFloat();
            else if (name() == "spin")
                spin = (readElementText() == "1");
            else if (name() == "spintrim")
                spinTrim = readElementText().toFloat();
            else if (name() == "overlap") {
                overlap = (Boats::Overlaps)FLAG_VALUE(Boats, Overlap, readElementText().toStdString().c_str());
            }
            else if (name() == "flag") {
                flag = (Boats::Flag)ENUM_VALUE(Boats, Flag, readElementText().toStdString().c_str());
            }
            else if (name() == "hidden") {
                hidden = (readElementText() == "1");
            }
            else if (name() == "acceleration") {
                acceleration = (Boats::Acceleration)ENUM_VALUE(Boats, Acceleration, readElementText().toStdString().c_str());
            }
            else if (name() == "bubble_x")
                textPos.setX(readElementText().toFloat());
            else if (name() == "bubble_y")
                textPos.setY(readElementText().toFloat());
            else if (name() == "bubble_text")
                text = readElementText();
            else if (name() == "laylines")
                laylines = (readElementText() == "1");
            else
                discarded.append(readUnknownElement());
        }
    }
    BoatModel *boat;
    if (track->size() == 0) {
        boat = new BoatModel(track);
        boat->setPosition(pos);
        boat->setHeading(heading);
        track->addBoat(boat);
    } else {
        AddBoatUndoCommand *command = new AddBoatUndoCommand(track, pos, heading);
        situation->undoStack()->push(command);
        boat = command->boat();
    }
    boat->setTrim(trim);
    boat->setJibTrim(jibTrim);
    boat->setSpin(spin);
    boat->setSpinTrim(spinTrim);
    boat->setOverlap(overlap);
    boat->setFlag(flag);
    boat->setHidden(hidden);
    boat->setAcceleration(acceleration);
    boat->setTextPosition(textPos);
    boat->setText(text);
    boat->setLaylines(laylines);
    foreach (const QString elem, discarded) {
        boat->appendDiscardedXml(elem);
    }
}

void XmlSituationReader::readMark(SituationModel *situation) {
    QPointF pos;
    QColor color;
    bool zone = false;
    int length = 0;
    QPointF textPos(10,10);
    QString text;
    bool laylines = false;
    qreal heading = 0;
    bool arrowVisible = false;
    bool leaveToPort = true;
    bool labelVisible = true;
    QString labelText;
    QStringList discarded;
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;
        if (isStartElement()) {
            if (name() == "x")
                pos.setX(readElementText().toFloat());
            else if (name() == "y")
                pos.setY(readElementText().toFloat());
            else if (name() == "color")
                color.setNamedColor(readElementText());
            else if (name() == "zone")
                zone = (readElementText() == "1");
            else if (name() == "length")
                length = (readElementText().toInt());
            else if (name() == "bubble_x")
                textPos.setX(readElementText().toFloat());
            else if (name() == "bubble_y")
                textPos.setY(readElementText().toFloat());
            else if (name() == "bubble_text")
                text = readElementText();
            else if (name() == "laylines")
                laylines = (readElementText() == "1");
            else if (name() == "heading")
                heading = (readElementText().toFloat());
            else if (name() == "arrowVisible")
                arrowVisible = (readElementText() == "1");
            else if (name() == "leaveToPort")
                leaveToPort = (readElementText() == "1");
            else if (name() == "labelVisible")
                labelVisible = (readElementText() == "1");
            else if (name() == "labelText")
                labelText = (readElementText());
            else
                discarded.append(readUnknownElement());
        }
    }
    MarkModel *mark = situation->createMark(pos);
    mark->setColor(color);
    mark->setZone(zone);
    if (length != 0) {
        mark->setLength(length);
    }
    mark->setTextPosition(textPos);
    mark->setText(text);
    mark->setLaylines(laylines);
    mark->setHeading(heading);
    mark->setArrowVisible(arrowVisible);
    mark->setLeaveToPort(leaveToPort);
    mark->setLabelVisible(labelVisible);
    mark->setLabelText(labelText);
    foreach (const QString elem, discarded) {
        mark->appendDiscardedXml(elem);
    }
}

void XmlSituationReader::readPolyLine(SituationModel *situation) {
    QStringList discarded;
    AddPolyLineUndoCommand *command = new AddPolyLineUndoCommand(situation);
    situation->undoStack()->push(command);
    PolyLineModel *polyLine = command->polyLine();
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;
        if (isStartElement()) {
            if (name() == "point")
                readPoint(situation, polyLine);
            else
                polyLine->appendDiscardedXml(readUnknownElement());
        }
    }
}

void XmlSituationReader::readPoint(SituationModel *situation, PolyLineModel *polyLine) {
    QPointF pos;
    QPointF textPos(10,10);
    QString text;
    bool laylines = 0;
    QStringList discarded;
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;
        if (isStartElement()) {
            if (name() == "x")
                pos.setX(readElementText().toFloat());
            else if (name() == "y")
                pos.setY(readElementText().toFloat());
            else if (name() == "bubble_x")
                textPos.setX(readElementText().toFloat());
            else if (name() == "bubble_y")
                textPos.setY(readElementText().toFloat());
            else if (name() == "bubble_text")
                text = readElementText();
            else if (name() == "laylines")
                laylines = (readElementText() == "1");
            else
                discarded.append(readUnknownElement());
        }
    }
    PointModel *point = situation->createPoint(pos);
    point->setTextPosition(textPos);
    point->setText(text);
    point->setLaylines(laylines);
    foreach (const QString elem, discarded) {
        point->appendDiscardedXml(elem);
    }
}

void XmlSituationReader::readWind(SituationModel *situation) {
    bool visible = false;
    QPointF pos;
    QList<qreal> winds;
    QStringList discarded;
    while (!atEnd()) {
        readNext();
        if (isEndElement())
            break;
        if (isStartElement()) {
            if (name() == "visible")
                visible = (readElementText() == "1");
            else if (name() == "x")
                pos.setX(readElementText().toFloat());
            else if (name() == "y")
                pos.setY(readElementText().toFloat());
            else if (name() == "direction") {
                winds.append(readElementText().toFloat());
            }
            else
                discarded.append(readUnknownElement());
        }
    }
    foreach (const QString elem, discarded) {
        situation->wind().appendDiscardedXml(elem);
    }
    situation->wind().setVisible(visible);
    foreach (qreal heading, winds) {
        situation->undoStack()->push(new AddWindUndoCommand(&situation->wind(), heading));
    }
    situation->wind().setPosition(pos);
}

Boats::Series XmlSituationReader::series(const QString series) {
    int i;
    i = ENUM_VALUE(Boats, Series, series.toStdString().c_str());
    if (i != -1) {
        return (Boats::Series)i;
    } else {
        return Boats::keelboat;
    }
}
