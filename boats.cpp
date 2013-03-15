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
// By doing it this way guarantee that labels align correctly with enum tags irrespective of order
        m_seriesList[keelboat] = QObject::tr("Keelboat");
        m_seriesList[laser] = QObject::tr("Laser");
        m_seriesList[optimist] = QObject::tr("Optimist");
        m_seriesList[tornado] = QObject::tr("Tornado");
        m_seriesList[startboat] = QObject::tr("Committee boat");
        m_seriesList[rib] = QObject::tr("RIB");
    }
    return m_seriesList;
}
