//
// C++ Implementation: winddelegate
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2011 Thibaut GRIDEL
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

#include "winddelegate.h"

#include "commontypes.h"
#include "boats.h"

#include <QDoubleSpinBox>

extern int debugLevel;

QWidget * WindDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
    Q_UNUSED(option);
    if (debugLevel & 1 << DELEGATE) std::cout << "createEditor " << index.column() << std::endl;

    switch (index.column()) {
        case WIND_DIRECTION: {
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setMinimum(0);
            editor->setMaximum(359);
            editor->setDecimals(0);
            editor->setWrapping(true);
            return editor;
            }
            break;
        default:
            return 0;
            break;
    }
}

void WindDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const {
    if (debugLevel & 1 << DELEGATE) std::cout << "setEditorData " << index.column() << std::endl;

    switch (index.column()) {
        case WIND_DIRECTION: {
            QDoubleSpinBox *spin = static_cast<QDoubleSpinBox*>(editor);
            spin->setValue(index.data().value<qreal>());
            }
            break;
        default:
            QItemDelegate::setEditorData(editor, index);
            break;
    }
}

