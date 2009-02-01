//
// C++ Interface: XmlSituationWriter
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef XMLSITUATIONWRITER_H
#define XMLSITUATIONWRITER_H

#include <QXmlStreamWriter>

class SituationModel;
class TrackModel;
class BoatModel;
class MarkModel;

class XmlSituationWriter : public QXmlStreamWriter {
    public:
        XmlSituationWriter(SituationModel *situation);
        ~XmlSituationWriter();

        bool writeFile(QIODevice *device);

    private:
        void writeTrack(const TrackModel *track);
        void writeBoat(const BoatModel *boat);
        void writeMark(const MarkModel *mark);
        void writeUnknownElement(const QString &discarded);

        SituationModel *m_situation;
};

#endif
