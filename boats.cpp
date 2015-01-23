//
// C++ Implementation: boats
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2009 Thibaut GRIDEL
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

#include "boats.h"


QList<QString> Boats::m_seriesList;

QList<QString> Boats::seriesList() {
    if (!m_seriesList.size()) {
        for (int i=0; i< ENUM_SIZE(Boats, Series)-1; i++) {
            m_seriesList << ENUM_NAME(Boats, Series, i);
        }
    }
// By doing it this way guarantee that labels align correctly with enum tags irrespective of order
    m_seriesList[keelboat] = QObject::tr("Spin Keelboat");
    m_seriesList[keelboatwithgenn] = QObject::tr("Gennaker Keelboat");
    m_seriesList[int49er] = QObject::tr("49er");
    m_seriesList[rsfeva] = QObject::tr("RS Feva");
    m_seriesList[int470] = QObject::tr("470");
    m_seriesList[int420] = QObject::tr("420");
    m_seriesList[int29er] = QObject::tr("29er");
    m_seriesList[finn] = QObject::tr("Finn");
    m_seriesList[laser] = QObject::tr("Laser");
    m_seriesList[firefly] = QObject::tr("Firefly");
    m_seriesList[topper] = QObject::tr("Topper");
    m_seriesList[optimist] = QObject::tr("Optimist");
    m_seriesList[tornado] = QObject::tr("Tornado");
    m_seriesList[nacra17] = QObject::tr("Nacra 17");
    m_seriesList[startboat] = QObject::tr("Committee boat");
    m_seriesList[rib] = QObject::tr("RIB");
    return m_seriesList;
}

QList<int> Boats::m_seriesSizeList;

QList<int> Boats::seriesSizeList() {
    if (!m_seriesSizeList.size()) {
        for (int i=0; i< ENUM_SIZE(Boats, Series)-1; i++) {
            m_seriesSizeList << 100; // default size of 100
        }
        m_seriesSizeList[keelboat] = 100;
        m_seriesSizeList[keelboatwithgenn] = 100;
        m_seriesSizeList[int49er] = 48;
        m_seriesSizeList[rsfeva] = 36.4;
        m_seriesSizeList[int470] = 47;
        m_seriesSizeList[int420] = 42;
        m_seriesSizeList[int29er] = 41;
        m_seriesSizeList[finn] = 45;
        m_seriesSizeList[laser] = 40;
        m_seriesSizeList[firefly] = 36.6;
        m_seriesSizeList[topper] = 34;
        m_seriesSizeList[optimist] = 23;
        m_seriesSizeList[tornado] = 61;
        m_seriesSizeList[nacra17] = 52.5;
        m_seriesSizeList[rib] = 60;
    }
    return m_seriesSizeList;
}
