//
// C++ Implementation: situationprint
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2009-2012 Thibaut GRIDEL
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

#include "situationprint.h"

#include "commontypes.h"
#include "situationmodel.h"
#include "situationview.h"

extern int debugLevel;


SituationPrint::SituationPrint(SituationModel *situation, SituationView *view, QWidget *parent)
    : QTextEdit(parent),
    m_situation(situation),
    m_view(view) {
    setAcceptRichText(true);
}

void SituationPrint::render(QRectF pageRect) {
    Q_UNUSED(pageRect);
    if (!m_situation) {
        return;
    }
    QTextCursor cursor(document());

    QTextBlockFormat headingblock;
    headingblock.setAlignment(Qt::AlignHCenter);
    QTextCharFormat headingchar;
    headingchar.setFontPointSize(20);
    headingchar.setFontWeight(2);

    QTextBlockFormat descblock;
    descblock.setAlignment(Qt::AlignLeading);
    QTextCharFormat descchar;
    descchar.setFontPointSize(14);
    descchar.setFontWeight(2);

    QTextBlockFormat textblock;
    textblock.setAlignment(Qt::AlignLeading);
    textblock.setIndent(1);
    QTextCharFormat textchar;
    textchar.setFontPointSize(12);

    cursor.insertBlock(headingblock);
    cursor.insertText(m_situation->title(), headingchar);

    if (!m_situation->rules().isEmpty()) {
        cursor.insertBlock();
        cursor.insertBlock(descblock);
        cursor.insertText(tr("Rule") + " ", descchar);
        cursor.insertText(m_situation->rules(), textchar);
    }


    if (!m_situation->abstract().isEmpty()) {
        cursor.insertBlock();
        cursor.insertBlock(descblock);
        cursor.insertBlock();
        cursor.insertBlock(textblock);
        cursor.insertText(m_situation->abstract(), textchar);
    }

    cursor.insertBlock();
    cursor.insertBlock(headingblock);

    QPixmap image(rect().size());
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_view->render(&painter);

    document()->addResource(QTextDocument::ImageResource,
         QUrl("mydata://image.png"), QVariant(image));
    cursor.insertImage("mydata://image.png");

    if (!m_situation->description().isEmpty()) {
        cursor.insertBlock();
        cursor.insertBlock(descblock);
        cursor.insertBlock();
        cursor.insertBlock(textblock);
        cursor.insertText(m_situation->description(), textchar);
    }
}
