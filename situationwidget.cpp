//
// C++ Implementation: situationwidget
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

#include <QtGui>

#include "situationwidget.h"
#include "trackdelegate.h"
#include "winddelegate.h"

#include "situationmodel.h"

#include "undocommands.h"

extern int debugLevel;

SituationWidget::SituationWidget(QWidget *parent)
    : QTabWidget(parent),
    m_situation(0) {

    // Scenario layout
    scenarioFrame = new QFrame();
    scenarioLayout = new QVBoxLayout(scenarioFrame);

    // Options layout
    optionsGroup = new QGroupBox(scenarioFrame);
    optionsGroup->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Maximum);

    optionsForm = new QFormLayout(optionsGroup);

    seriesCombo = new QComboBox(optionsGroup);
    seriesLabel = new QLabel(optionsGroup);
    optionsForm->addRow(seriesLabel, seriesCombo);

    laylineCheck = new QCheckBox(optionsGroup);
    laylineCheckLabel = new QLabel(optionsGroup);
    optionsForm->addRow(laylineCheckLabel, laylineCheck);

    laylineSpin = new QSpinBox(optionsGroup);
    laylineSpinLabel = new QLabel(optionsGroup);
    optionsForm->addRow(laylineSpinLabel, laylineSpin);

    lengthSpin = new QSpinBox(optionsGroup);
    lengthSpinLabel = new QLabel(optionsGroup);
    optionsForm->addRow(lengthSpinLabel, lengthSpin);

    windCheck = new QCheckBox(optionsGroup);
    windCheckLabel = new QLabel(optionsGroup);
    optionsForm->addRow(windCheckLabel, windCheck);

    // Track layout
    trackGroup = new QGroupBox(scenarioFrame);
    trackLayout = new QGridLayout(trackGroup);
    trackTableModel = new TrackTableModel(m_situation);
    trackTableView = new QTableView(trackGroup);
    trackTableView->setItemDelegate(new TrackDelegate);
    trackTableView->verticalHeader()->hide();
    trackTableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    trackTableView->horizontalHeader()->setDefaultSectionSize(60);
    trackTableView->horizontalHeader()->setStretchLastSection(true);
    trackTableView->horizontalHeader()->setClickable(false);
    trackLayout->addWidget(trackTableView);

    // Wind layout
    windGroup = new QGroupBox(scenarioFrame);
    windLayout = new QGridLayout(windGroup);
    windTableModel = new WindTableModel(&m_situation->wind());
    windTableView = new QTableView(windGroup);
    windTableView->setItemDelegate(new WindDelegate);
    windTableView->verticalHeader()->hide();
    windTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    windTableView->horizontalHeader()->setClickable(false);
    windLayout->addWidget(windTableView);


    // last bricks
    scenarioLayout->addWidget(optionsGroup);
    scenarioLayout->addWidget(trackGroup);
    scenarioLayout->addWidget(windGroup);

    addTab(scenarioFrame,tr("Scenario"));


    // description group
    descriptionFrame = new QFrame();

    descriptionGrid = new QGridLayout(descriptionFrame);
    descriptionForm = new QFormLayout();
    descriptionGrid->addLayout(descriptionForm, 0, 0);

    titleEdit = new QLineEdit(descriptionFrame);
    titleLabel = new QLabel(descriptionFrame);
    descriptionForm->addRow(titleLabel, titleEdit);

    rulesEdit = new QLineEdit(descriptionFrame);
    rulesLabel = new QLabel(descriptionFrame);
    descriptionForm->addRow(rulesLabel, rulesEdit);

    abstractLabel = new QLabel(descriptionFrame);
    descriptionGrid->addWidget(abstractLabel,1,0);

    abstractEdit = new QPlainTextEdit(descriptionFrame);
    abstractEdit->setUndoRedoEnabled(false);
    abstractEdit->setContextMenuPolicy(Qt::NoContextMenu);
    descriptionGrid->addWidget(abstractEdit,2,0);

    descriptionLabel = new QLabel(descriptionFrame);
    descriptionGrid->addWidget(descriptionLabel,3,0);

    descriptionEdit = new QPlainTextEdit(descriptionFrame);
    descriptionEdit->setUndoRedoEnabled(false);
    descriptionEdit->setContextMenuPolicy(Qt::NoContextMenu);
    descriptionGrid->addWidget(descriptionEdit,4,0);

    addTab(descriptionFrame,tr("Description"));

}

void SituationWidget::changeEvent(QEvent *event) {
    if(event->type() == QEvent::LanguageChange) {
        optionsGroup->setTitle(tr("Options"));
        seriesLabel->setText(tr("Series"));
        laylineCheckLabel->setText(tr("Show Grid"));
        laylineSpinLabel->setText(tr("Layline Angle"));
        lengthSpinLabel->setText(tr("Zone Length"));
        windCheckLabel->setText(tr("Show Wind"));
        trackGroup->setTitle(tr("Tracks"));
        windGroup->setTitle(tr("Wind"));
        setTabText(0, tr("Scenario"));

        titleLabel->setText(tr("Title"));
        rulesLabel->setText(tr("Rules"));
        abstractLabel->setText(tr("Abstract"));
        descriptionLabel->setText(tr("Description"));
        setTabText(1, tr("Description"));
    } else {
        QTabWidget::changeEvent(event);
    }

}

void SituationWidget::update() {
    if (m_situation) {
        titleEdit->setText(m_situation->title());
        rulesEdit->setText(m_situation->rules());
        seriesCombo->setCurrentIndex(m_situation->situationSeries());
        laylineCheck->setChecked(m_situation->showLayline());
        laylineSpin->setValue(m_situation->laylineAngle());
        lengthSpin->setValue(m_situation->situationLength());
        windCheck->setChecked(m_situation->wind().visible());
        windGroup->setVisible(m_situation->wind().visible());
        abstractEdit->setPlainText(m_situation->abstract());
        descriptionEdit->setPlainText(m_situation->description());
    }
}

void SituationWidget::setSituation(SituationModel *situation) {
    if (situation != m_situation) {
        m_situation = situation;

        update();

        // Scenario Group
        connect (titleEdit, SIGNAL(editingFinished()),
                this, SLOT(setTitle()));
        connect (situation, SIGNAL(titleChanged(QString)),
                titleEdit, SLOT(setText(QString)));

        connect (rulesEdit, SIGNAL(editingFinished()),
                this, SLOT(setRules()));
        connect (situation, SIGNAL(rulesChanged(QString)),
                rulesEdit, SLOT(setText(QString)));

        seriesCombo->addItems(Boats::seriesList());
        connect (seriesCombo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(setSeries(int)));
        connect (situation, SIGNAL(seriesChanged(int)),
                seriesCombo, SLOT(setCurrentIndex(int)));

        connect(laylineCheck, SIGNAL(toggled(bool)),
                this, SLOT(setShowLayline(bool)));
        connect(situation, SIGNAL(showLaylineChanged(bool)),
                laylineCheck, SLOT(setChecked(bool)));

        laylineSpin->setRange(0, 359);
        laylineSpin->setWrapping(true);
        connect (laylineSpin, SIGNAL(valueChanged(int)),
                this, SLOT(setLayline(int)));
        connect (situation, SIGNAL(laylineChanged(const int)),
                laylineSpin, SLOT(setValue(int)));

        lengthSpin->setRange(1,5);
        connect (lengthSpin, SIGNAL(valueChanged(int)),
                this, SLOT(setLength(int)));
        connect (situation, SIGNAL(lengthChanged(const int)),
                lengthSpin, SLOT(setValue(int)));

        connect(windCheck, SIGNAL(toggled(bool)),
                this, SLOT(setShowWind(bool)));
        connect(&situation->wind(), SIGNAL(windVisibleChanged(bool)),
                windCheck, SLOT(setChecked(bool)));

        connect(windCheck, SIGNAL(toggled(bool)),
                windGroup, SLOT(setVisible(bool)));

        connect(abstractEdit->document(), SIGNAL(contentsChanged()),
                this, SLOT(setAbstract()));
        connect(situation, SIGNAL(abstractChanged(const QString)),
                this, SLOT(updateAbstract(const QString)));

        connect(descriptionEdit->document(), SIGNAL(contentsChanged()),
                this, SLOT(setDescription()));
        connect(situation, SIGNAL(descriptionChanged(const QString)),
                this, SLOT(updateDescription(const QString)));

        // Track group
        trackTableModel->setSituation(m_situation);
        connect(situation, SIGNAL(trackAdded(TrackModel*)),
                trackTableModel, SLOT(addTrack(TrackModel*)));
        connect(situation, SIGNAL(trackRemoved(TrackModel*)),
                trackTableModel, SLOT(deleteTrack(TrackModel*)));
        trackTableView->setModel(trackTableModel);

        // Wind group
        windTableModel->setWind(&m_situation->wind());
        connect(&m_situation->wind(), SIGNAL(windReset()),
                windTableModel, SLOT(updateWind()));
        windTableView->setModel(windTableModel);
    }
}

void SituationWidget::unSetSituation() {

    // Scenario Group
    disconnect(titleEdit, 0, 0, 0);
    titleEdit->clear();

    disconnect(rulesEdit, 0, 0, 0);
    rulesEdit->clear();

    disconnect(seriesCombo, 0, 0, 0);
    disconnect(m_situation, 0, seriesCombo, 0);
    seriesCombo->clear();
    seriesCombo->setCurrentIndex(0);

    disconnect(laylineCheck, 0, 0, 0);
    disconnect(m_situation, 0, laylineCheck, 0);
    laylineCheck->setChecked(true);

    disconnect(laylineSpin,  0, 0, 0);
    disconnect(m_situation, 0, laylineSpin, 0);
    laylineSpin->setValue(40);

    disconnect(lengthSpin, 0, 0, 0);
    disconnect(m_situation, 0, lengthSpin, 0);
    lengthSpin->setValue(3);

    disconnect(windCheck, 0, 0, 0);
    disconnect(&m_situation->wind(), 0, windCheck, 0);
    windCheck->setChecked(false);

    disconnect(m_situation, 0, this, 0);
    disconnect(abstractEdit->document(), 0, 0, 0);
    abstractEdit->clear();
    disconnect(descriptionEdit->document(), 0, 0, 0);
    descriptionEdit->clear();

    // Track Group
    disconnect(m_situation, 0, trackTableModel, 0);
    disconnect(&m_situation->wind(), 0, windTableModel, 0);

    m_situation = 0;
}

void SituationWidget::setTitle() {
    if (m_situation) {
        if (titleEdit->text() != m_situation->title()) {
            m_situation->undoStack()->push(new SetTitleUndoCommand(m_situation, titleEdit->text()));
        }
    }
}

void SituationWidget::setRules() {
    if (m_situation) {
        if (rulesEdit->text() != m_situation->rules()) {
            m_situation->undoStack()->push(new SetRulesUndoCommand(m_situation, rulesEdit->text()));
        }
    }
}

void SituationWidget::setShowLayline(bool show) {
    if (m_situation) {
        if (show != m_situation->showLayline())
            m_situation->undoStack()->push(new SetShowLaylineUndoCommand(m_situation));
    }
}

void SituationWidget::setLayline(int angle) {
    if (m_situation) {
        if (angle != m_situation->laylineAngle()) {
            m_situation->undoStack()->push(new SetLaylineUndoCommand(m_situation, angle));
        }
    }
}

void SituationWidget::setLength(int length) {
    if (m_situation) {
        if (length != m_situation->situationLength()) {
            m_situation->undoStack()->push(new LengthMarkUndoCommand(m_situation, length));
        }
    }
}

void SituationWidget::setShowWind(bool show) {
    if (m_situation) {
        if (show != m_situation->wind().visible())
            m_situation->undoStack()->push(new SetShowWindUndoCommand(&m_situation->wind()));
    }
}

void SituationWidget::setSeries(int series) {
    if (m_situation) {
        if (series != m_situation->situationSeries()) {
            m_situation->undoStack()->push(new SetSituationSeriesUndoCommand(m_situation, series));
        }
    }
}

void SituationWidget::setAbstract() {
    if (m_situation) {
        if (abstractEdit->document()->toPlainText() != m_situation->abstract()) {
            m_situation->undoStack()->push(new SetAbstractUndoCommand(m_situation, abstractEdit->document()->toPlainText()));
        }
    }
}

void SituationWidget::setDescription() {
    if (m_situation) {
        if (descriptionEdit->document()->toPlainText() != m_situation->description()) {
            m_situation->undoStack()->push(new SetDescriptionUndoCommand(m_situation, descriptionEdit->document()->toPlainText()));
        }
    }
}

void SituationWidget::updateAbstract(const QString abstract) {
    if (m_situation) {
        if (abstractEdit->document()->toPlainText() != abstract) {
            disconnect(abstractEdit->document(), 0, 0, 0);
            abstractEdit->document()->setPlainText(abstract);
            connect(abstractEdit->document(), SIGNAL(contentsChanged()),
                    this, SLOT(setAbstract()));
        }
    }
}

void SituationWidget::updateDescription(const QString description) {
    if (m_situation) {
        if (descriptionEdit->document()->toPlainText() != description) {
            disconnect(descriptionEdit->document(), 0, 0, 0);
            descriptionEdit->document()->setPlainText(description);
            connect(descriptionEdit->document(), SIGNAL(contentsChanged()),
                    this, SLOT(setDescription()));
        }
    }
}

