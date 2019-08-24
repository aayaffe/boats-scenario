//
// C++ Interface: situationwidget
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
#ifndef SITUATIONWIDGET_H
#define SITUATIONWIDGET_H


#include "tracktablemodel.h"
#include "windtablemodel.h"

#include <QTableWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFormLayout>
#include <QPlainTextEdit>

class SituationModel;

class SituationWidget : public QTabWidget {
    Q_OBJECT
    public:
        SituationWidget(QWidget *parent = 0);
        ~SituationWidget() {}

        void update();

    public slots:
        void setSituation(SituationModel *situation);
        void unSetSituation();

        // set the model from widgets
        void setTitle();
        void setRules();
        void setShowLayline(bool);
        void setLayline(int angle);
        void setSeries(int series);
        void setLength(int length);
        void setShowWind(bool);
        void setAbstract();
        void setDescription();

        // set the widget from the model
        void updateAbstract(const QString abstract);
        void updateDescription(const QString description);

    protected:
        void changeEvent(QEvent *event);

    private:
        SituationModel *m_situation;

        QFrame *scenarioFrame;
        QVBoxLayout *scenarioLayout;

        QGroupBox *optionsGroup;
        QComboBox *seriesCombo;
        QLabel *seriesLabel;
        QCheckBox *laylineCheck;
        QLabel *laylineCheckLabel;
        QSpinBox *laylineSpin;
        QLabel *laylineSpinLabel;
        QSpinBox *lengthSpin;
        QLabel *lengthSpinLabel;
        QCheckBox *windCheck;
        QLabel *windCheckLabel;
        QFormLayout *optionsForm;

        QGroupBox *trackGroup;
        TrackTableModel *trackTableModel;
        QTableView *trackTableView;
        QGridLayout *trackLayout;

        QGroupBox *windGroup;
        WindTableModel *windTableModel;
        QTableView *windTableView;
        QGridLayout *windLayout;

        QFrame *descriptionFrame;
        QLineEdit *titleEdit;
        QLabel *titleLabel;
        QLineEdit *rulesEdit;
        QLabel *rulesLabel;
        QPlainTextEdit *abstractEdit;
        QLabel *abstractLabel;
        QPlainTextEdit *descriptionEdit;
        QLabel *descriptionLabel;
        QGridLayout *descriptionGrid;
        QFormLayout *descriptionForm;

};

#endif
