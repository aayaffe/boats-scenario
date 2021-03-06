//
// C++ Implementation: trackdelegate
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
#include <iostream>

#include "trackdelegate.h"

#include "colorpickerwidget.h"

#include "commontypes.h"
#include "boats.h"

#include <QPainter>

extern int debugLevel;

void TrackDelegate::paint(QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {

    painter->save();

    drawBackground(painter, option, index);

    switch (index.column()) {
        case TRACK_COLOR: {
            QColor trackColor = index.data().value<QColor>();
            painter->setPen(Qt::NoPen);
            painter->setBrush(trackColor);
            qreal height = option.rect.height() / 8.0;
            qreal width = option.rect.width() / 8.0;
            QRectF rect(option.rect);
            rect.adjust(width, height, -width, -height);
            painter->drawRoundedRect(rect, width, height);
            }
            break;
        case TRACK_SERIES: {
            int series = index.data().value<int>();
            drawDisplay(painter, option, option.rect, Boats::seriesList().at(series));
            }
            break;
        default:
            QItemDelegate::paint(painter, option, index);
            break;
    }

    drawFocus(painter, option, option.rect);
    painter->restore();
}

QSize TrackDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
    return QItemDelegate::sizeHint(option, index);
}

QWidget * TrackDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
    Q_UNUSED(option);
    if (debugLevel & 1 << DELEGATE) std::cout << "createEditor " << index.column() << std::endl;

    switch (index.column()) {
        case TRACK_COLOR: {
            ColorPickerWidget *editor = new ColorPickerWidget(parent);
            connect(editor, SIGNAL(editingFinished()),
                    this, SLOT(commitAndCloseColor()), Qt::QueuedConnection);
            return editor;
            }
            break;
        case TRACK_SERIES: {
            QComboBox *editor = new QComboBox(parent);
            editor->addItems(Boats::seriesList());
            connect(editor, SIGNAL(activated(int)),
                    this, SLOT(commitAndCloseCombo()));
            return editor;
            }
            break;
        default:
            return 0;
            break;
    }
}

void TrackDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const {
    if (debugLevel & 1 << DELEGATE) std::cout << "setEditorData " << index.column() << std::endl;

    switch (index.column()) {
        case TRACK_COLOR: {
            ColorPickerWidget *colorEditor = getColorEditor(editor);
            QColor color = index.data().value<QColor>();
            colorEditor->setColor(color);
            }
            break;
        case TRACK_SERIES: {
            QComboBox *seriesEditor = getComboEditor(editor);
            int series = index.data().value<int>();
            seriesEditor->setCurrentIndex(series);
            }
            break;
        default:
            QItemDelegate::setEditorData(editor, index);
            break;
    }
}

void TrackDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model,
    const QModelIndex &index) const {
    if (debugLevel & 1 << DELEGATE) std::cout << "setModelData " << index.column() << std::endl;

    switch (index.column()) {
        case TRACK_COLOR: {
            ColorPickerWidget *colorEditor = getColorEditor(editor);
            QColor color = colorEditor->color();
            model->setData(index, qVariantFromValue(color));
            }
            break;
        case TRACK_SERIES: {
            QComboBox *seriesEditor = getComboEditor(editor);
            int series = seriesEditor->currentIndex();
            model->setData(index, qVariantFromValue(series));
            }
            break;
        default:
            QItemDelegate::setModelData(editor, model, index);
            break;
    }
}

void TrackDelegate::commitAndCloseColor() {
    if (debugLevel & 1 << DELEGATE) std::cout << "commitAndCloseColor" << std::endl;

    ColorPickerWidget *colorEditor = getColorEditor(sender());
    emit commitData(colorEditor);
    emit closeEditor(colorEditor);
}

void TrackDelegate::commitAndCloseCombo() {
    if (debugLevel & 1 << DELEGATE) std::cout << "commitAndCloseCombo" << std::endl;

    QComboBox *pathEditor = getComboEditor(sender());
    emit commitData(pathEditor);
    emit closeEditor(pathEditor);
}

ColorPickerWidget * TrackDelegate::getColorEditor(QObject *editor) const {
    return qobject_cast<ColorPickerWidget *>(editor);
}

QComboBox * TrackDelegate::getComboEditor(QObject *editor) const {
    return qobject_cast<QComboBox *>(editor);
}
