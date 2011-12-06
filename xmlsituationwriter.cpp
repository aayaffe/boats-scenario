//
// C++ Implementation: XmlSituationWriter
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

#include "xmlsituationwriter.h"

#include <iostream>

#include "commontypes.h"
#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"

extern int debugLevel;

XmlSituationWriter::XmlSituationWriter(SituationModel *situation)
        : m_situation(situation) {
    setAutoFormatting(true);
    if (debugLevel & 1 << XML) std::cout << "new XmlSituationWriter" << std::endl;
}

XmlSituationWriter::~XmlSituationWriter() {}

bool XmlSituationWriter::writeFile(QIODevice *device) {
    setDevice(device);
    if (debugLevel & 1 << XML) std::cout << "WriteFile" << std::endl;

    writeStartDocument();
    writeDTD("<!DOCTYPE xmlscenario>");
    writeStartElement("xmlscenario");
    writeAttribute("version", "1.0");
    if (!m_situation->title().isEmpty()) {
        writeTextElement("title", m_situation->title());
    }
    if (!m_situation->rules().isEmpty()) {
        writeTextElement("rules", m_situation->rules());
    }
    if (!m_situation->abstract().isEmpty()) {
        writeTextElement("abstract", m_situation->abstract());
    }
    if (!m_situation->description().isEmpty()) {
        writeTextElement("description", m_situation->description());
    }
    writeTextElement("series",ENUM_NAME(Boats, Series, m_situation->situationSeries()));
    writeTextElement("showlayline",QString::number(m_situation->showLayline()));
    writeTextElement("layline",QString::number(m_situation->laylineAngle()));
    writeTextElement("length",QString::number(m_situation->situationLength()));
    writeWind(m_situation->wind());
    foreach (const QString discarded, m_situation->discardedXml())
        writeUnknownElement(discarded);
    foreach (const MarkModel *mark, m_situation->marks())
        writeMark(mark);
    foreach (const TrackModel *track, m_situation->tracks())
        writeTrack(track);
    foreach (const PolyLineModel *polyLine, m_situation->polyLines())
        writePolyLine(polyLine);

    writeEndDocument();
    if (debugLevel & 1 << XML) std::cout << "done WriteFile" << std::endl;
    return true;
}

void XmlSituationWriter::writeTrack(const TrackModel *track) {
    if (debugLevel & 1 << XML) {
        std::cout << "WriteTrack" << std::endl;
        std::cout << " color=" << track->color().name().toStdString() << std::endl;
        std::cout << " series=" << ENUM_NAME(Boats, Series, track->series()) << std::endl;
    }
    writeStartElement("track");
    writeTextElement("color",track->color().name());
    writeTextElement("series",ENUM_NAME(Boats, Series, track->series()));
    if (!track->showPath()) {
        writeTextElement("path",QString::number(track->showPath()));
    }
    foreach (const QString discarded, track->discardedXml())
        writeUnknownElement(discarded);
    foreach (const BoatModel *boat, track->boats())
        writeBoat(boat);
    writeEndElement();
}

void XmlSituationWriter::writeBoat(const BoatModel *boat) {
    if (debugLevel & 1 << XML) {
        std::cout << "WriteBoat" << std::endl;
        std::cout << " x=" << boat->position().x() << std::endl;
        std::cout << " y=" << boat->position().y() << std::endl;
        std::cout << " heading=" << boat->heading() << std::endl;
    }
    writeStartElement("boat");
    writeTextElement("x",QString::number(boat->position().x()));
    writeTextElement("y",QString::number(boat->position().y()));
    writeTextElement("heading",QString::number(boat->heading()));
    if (boat->trim() != 0) {
        writeTextElement("trim",QString::number(boat->trim()));
    }
    if (boat->spin()) {
        writeTextElement("spin",QString::number(boat->spin()));
    }
    if (boat->overlap() != Boats::none) {
        writeTextElement("overlap", FLAG_NAME(Boats, Overlap, boat->overlap()));
    }
    if (boat->flag() != Boats::noFlag) {
        writeTextElement("flag", ENUM_NAME(Boats, Flag, boat->flag()));
    }
    if (boat->hidden()) {
        writeTextElement("hidden",QString::number(boat->hidden()));
    }
    if (boat->acceleration() != Boats::constant) {
        writeTextElement("acceleration", ENUM_NAME(Boats, Acceleration, boat->acceleration()));
    }
    if (!boat->text().isEmpty()) {
        writeTextElement("bubble_x",QString::number(boat->textPosition().x()));
        writeTextElement("bubble_y",QString::number(boat->textPosition().y()));
        writeTextElement("bubble_text",boat->text());
    }
    foreach (const QString discarded, boat->discardedXml())
        writeUnknownElement(discarded);
    writeEndElement();
}

void XmlSituationWriter::writeMark(const MarkModel *mark) {
    if (debugLevel & 1 << XML) {
        std::cout << "WriteMark" << std::endl;
        std::cout << " x=" << mark->position().x() << std::endl;
        std::cout << " y=" << mark->position().y() << std::endl;
        std::cout << " color=" << mark->color().name().toStdString() << std::endl;
        std::cout << " zone=" << mark->zone() << std::endl;
        std::cout << " length=" << mark->length() << std::endl;
    }
    writeStartElement("mark");
    writeTextElement("x",QString::number(mark->position().x()));
    writeTextElement("y",QString::number(mark->position().y()));
    writeTextElement("color",mark->color().name());
    if (mark->zone()) {
        writeTextElement("zone",QString::number(mark->zone()));
    }
    if (mark->length() != mark->situation()->situationLength()) {
        writeTextElement("length",QString::number(mark->length()));
    }
    foreach (const QString discarded, mark->discardedXml())
        writeUnknownElement(discarded);
    writeEndElement();
}

void XmlSituationWriter::writePolyLine(const PolyLineModel *polyLine) {
    if (debugLevel & 1 << XML) {
        std::cout << "WritePolyLine" << std::endl;
        std::cout << " points=" << polyLine->size() << std::endl;
    }
    writeStartElement("polyline");
    foreach (const QString discarded, polyLine->discardedXml())
        writeUnknownElement(discarded);
    foreach (const PointModel *point, polyLine->points()) {
        writePoint(point);
    }
    writeEndElement();
}

void XmlSituationWriter::writePoint(const PointModel *point) {
    if (debugLevel & 1 << XML) {
        std::cout << "WritePoint" << std::endl;
        std::cout << " x=" << point->position().x() << std::endl;
        std::cout << " y=" << point->position().y() << std::endl;
    }

    writeStartElement("point");
    foreach (const QString discarded, point->discardedXml())
        writeUnknownElement(discarded);
    writeTextElement("x",QString::number(point->position().x()));
    writeTextElement("y",QString::number(point->position().y()));
    writeEndElement();
}

void XmlSituationWriter::writeWind(const WindModel &wind) {
    if (debugLevel & 1 << XML) {
        std::cout << "WritePoint" << std::endl;
    }
    writeStartElement("wind");
    foreach (const QString discarded, wind.discardedXml())
        writeUnknownElement(discarded);
    writeTextElement("x",QString::number(wind.position().x()));
    writeTextElement("y",QString::number(wind.position().y()));
    for(int i = 0; i < wind.size(); ++i) {
        writeTextElement("direction", QString::number(wind.windAt(i)));
    }
    writeEndElement();
}

void XmlSituationWriter::writeUnknownElement(const QString &discarded) {
    if (debugLevel & 1 << XML) std::cout << "WriteUnknownElement" << std::endl;
    if (!discarded.isEmpty()) {
        QXmlStreamReader reader(discarded);
        while (!reader.atEnd()) {
            reader.readNext();
            if (reader.isStartElement()
                || reader.isEndElement()
                || reader.isCharacters()) {
                writeCurrentToken(reader);
            }
        }
    }
}
