//
// C++ Implementation: mainwindow
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

#include "mainwindow.h"

#include "commontypes.h"
#include "boats.h"

#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"

#include "scenarioanimation.h"

#include "undocommands.h"
#include "xmlsituationreader.h"
#include "xmlsituationwriter.h"

#include "situationwidget.h"
#include "situationscene.h"
#include "situationview.h"

#ifdef GIF_EXPORT
#include "gifwriter.h"
#endif

extern int debugLevel;

const int MainWindow::maxRecent(5);

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
        menubar(new QMenuBar(this)),
        toolbar(new QToolBar(this)),
        tabWidget(new QTabWidget(this)),
        animationBar(new QToolBar(this)),
        situationDock(new QDockWidget(this)),
        situationWidget(new SituationWidget(situationDock)),
        statusbar(new QStatusBar(this)),
        qtTranslator(new QTranslator(this)),
        translator(new QTranslator(this)) {

    // Actions
    createActions();

    // Bars
    createMenus();
    setMenuBar(menubar);
    addToolBar(toolbar);
    addToolBar(Qt::BottomToolBarArea, animationBar);
    setStatusBar(statusbar);

    // Docks
    createDocks();
    addDockWidget(Qt::LeftDockWidgetArea, situationDock);

    // View
    newTabButton = new QPushButton(QIcon(":/images/tab_new.png"),QString(""), this);
    connect(newTabButton, SIGNAL(clicked()),
            newTabAction, SLOT(trigger()));
    tabWidget->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    removeTabButton = new QPushButton(QIcon(":/images/tab_remove.png"),QString(""), this);
    removeTabButton->setEnabled(removeTabAction->isEnabled());
    connect(removeTabButton, SIGNAL(clicked()),
            removeTabAction, SLOT(trigger()));
    tabWidget->setCornerWidget(removeTabButton, Qt::TopRightCorner);
    connect(tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(setTab(int)));
    newTab();
    setCentralWidget(tabWidget);

    readSettings();

    // Locale and translation setup
    createTranslations(QLocale::system().name());
}

MainWindow::~MainWindow() {}

void MainWindow::createTranslations(QString locale) {

    qtTranslator->load("qt_" + locale,
                       QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(qtTranslator);

    if (translator->load(QString("boats_").append(locale).append(".qm"), TRANSLATEDIR)) {
        qApp->installTranslator(translator);
    } else {
        QCoreApplication::postEvent(QCoreApplication::instance(),
                                    new QEvent(QEvent::LanguageChange));
    }
}

void MainWindow::createActions() {
    newFileAction = new QAction(this);
    newFileAction->setIcon(QIcon(":/images/filenew.png"));
    connect(newFileAction, SIGNAL(triggered()),
            this, SLOT(newFile()));

    openFileAction = new QAction(this);
    openFileAction->setIcon(QIcon(":/images/fileopen.png"));
    connect(openFileAction, SIGNAL(triggered()),
            this, SLOT(openFile()));

    saveFileAction = new QAction(this);
    saveFileAction->setIcon(QIcon(":/images/filesave.png"));
    saveFileAction->setEnabled(false);
    connect(saveFileAction, SIGNAL(triggered()),
            this, SLOT(saveFile()));

    saveAsAction = new QAction(this);
    saveAsAction->setIcon(QIcon(":/images/filesaveas.png"));
    saveAsAction->setEnabled(false);
    connect(saveAsAction, SIGNAL(triggered()),
            this, SLOT(saveAs()));

    newTabAction = new QAction(this);
    newTabAction->setIcon(QIcon(":/images/tab_new.png"));
    connect(newTabAction, SIGNAL(triggered()),
            this, SLOT(newTab()));

    removeTabAction = new QAction(this);
    removeTabAction->setIcon(QIcon(":/images/tab_remove.png"));
    removeTabAction->setEnabled(false);
    connect(removeTabAction, SIGNAL(triggered()),
            this, SLOT(removeTab()));

    restoreFilesAction = new QAction(this);
    connect(restoreFilesAction, SIGNAL(triggered()),
            this, SLOT(restoreFiles()));

    printAction = new QAction(this);
    printAction->setIcon(QIcon(":images/fileprint.png"));
    connect(printAction, SIGNAL(triggered()),
            this, SLOT(print()));

    printPreviewAction = new QAction(this);
    printPreviewAction->setIcon(QIcon(":images/filequickprint.png"));
    connect(printPreviewAction, SIGNAL(triggered()),
            this, SLOT(printPreview()));

    exportPdfAction = new QAction(this);
    exportPdfAction->setIcon(QIcon(":/images/pdf.png"));
    connect(exportPdfAction, SIGNAL(triggered()),
            this, SLOT(exportPdf()));

    exportImageAction = new QAction(this);
    exportImageAction->setIcon(QIcon(":/images/export.png"));
    connect(exportImageAction, SIGNAL(triggered()),
            this, SLOT(exportImage()));

#ifdef GIF_EXPORT
    exportAnimationAction = new QAction(this);
    exportAnimationAction->setIcon(QIcon(":/images/video.png"));
    connect(exportAnimationAction, SIGNAL(triggered()),
            this, SLOT(exportAnimation()));
#endif

    exitAction = new QAction(this);
    connect(exitAction, SIGNAL(triggered()),
        this, SLOT(close()));

    addTrackAction = new QAction(this);
    addTrackAction->setIcon(QIcon(":/images/addtrack.png"));
    addTrackAction->setCheckable(true);
    connect(addTrackAction, SIGNAL(triggered()),
            this, SLOT(addTrack()));

    addBoatAction = new QAction(this);
    addBoatAction->setIcon(QIcon(":/images/addboat.png"));
    addBoatAction->setCheckable(true);
    connect(addBoatAction, SIGNAL(triggered()),
            this, SLOT(addBoat()));

    addMarkAction = new QAction(this);
    addMarkAction->setIcon(QIcon(":/images/addmark.png"));
    addMarkAction->setCheckable(true);
    connect(addMarkAction, SIGNAL(triggered()),
            this, SLOT(addMark()));

    addPolyLineAction = new QAction(this);
    addPolyLineAction->setIcon(QIcon(":/images/addpoly.png"));
    addPolyLineAction->setCheckable(true);
    connect(addPolyLineAction, SIGNAL(triggered()),
            this, SLOT(addPolyLine()));

    addPointAction = new QAction(this);
    addPointAction->setIcon(QIcon(":/images/addpoint.png"));
    addPointAction->setCheckable(true);
    connect(addPointAction, SIGNAL(triggered()),
            this, SLOT(addPoint()));

    trimSailAction = new QAction(this);
    connect(trimSailAction, SIGNAL(triggered()),
            this, SLOT(trimSail()));

    autotrimSailAction = new QAction(this);
    connect(autotrimSailAction, SIGNAL(triggered()),
            this, SLOT(autotrimSail()));

    untrimSailAction = new QAction(this);
    connect(untrimSailAction, SIGNAL(triggered()),
            this, SLOT(untrimSail()));

    togglePortOverlapAction = new QAction(this);
    togglePortOverlapAction->setCheckable(true);
    connect(togglePortOverlapAction, SIGNAL(triggered()),
            this, SLOT(togglePortOverlap()));

    toggleStarboardOverlapAction = new QAction(this);
    toggleStarboardOverlapAction->setCheckable(true);
    connect(toggleStarboardOverlapAction, SIGNAL(triggered()),
            this, SLOT(toggleStarboardOverlap()));

    toggleHiddenAction = new QAction(this);
    toggleHiddenAction->setCheckable(true);
    connect(toggleHiddenAction, SIGNAL(triggered()),
            this, SLOT(toggleHidden()));

    toggleTextAction = new QAction(this);
    toggleTextAction->setCheckable(true);
    connect(toggleTextAction, SIGNAL(triggered()),
            this, SLOT(toggleText()));

    toggleSpinAction = new QAction(this);
    toggleSpinAction->setCheckable(true);
    toggleSpinAction->setEnabled(false);
    connect(toggleSpinAction, SIGNAL(triggered()),
            this, SLOT(toggleSpin()));

    toggleMarkZoneAction = new QAction(this);
    toggleMarkZoneAction->setIcon(QIcon(":/images/zone.png"));
    connect(toggleMarkZoneAction, SIGNAL(triggered()),
            this, SLOT(toggleMarkZone()));

    toggleLaylinesAction = new QAction(this);
    toggleLaylinesAction->setIcon(QIcon(":/images/laylines.png"));
    toggleLaylinesAction->setCheckable(true);
    connect(toggleLaylinesAction, SIGNAL(triggered()),
            this, SLOT(toggleLaylines()));

    deleteTrackAction = new QAction(this);
    connect(deleteTrackAction, SIGNAL(triggered()),
            this, SLOT(deleteTrack()));

    deleteAction = new QAction(this);
    connect(deleteAction, SIGNAL(triggered()),
            this, SLOT(deleteModels()));

    animateAction = new QAction(this);
    animateAction->setIcon(QIcon(":/images/animate.png"));
    animateAction->setCheckable(true);
    connect(animateAction, SIGNAL(toggled(bool)),
            this, SLOT(animate(bool)));

    startAction  = new QAction(this);
    startAction->setIcon(QIcon(":/images/player_play.png"));
    startAction->setEnabled(false);
    connect(startAction, SIGNAL(triggered()),
            this, SLOT(play()));

    pauseAction  = new QAction(this);
    pauseAction->setIcon(QIcon(":/images/player_pause.png"));
    pauseAction->setEnabled(false);
    pauseAction->setCheckable(true);
    connect(pauseAction, SIGNAL(toggled(bool)),
            this, SLOT(pause(bool)));

    stopAction = new QAction(this);
    stopAction->setIcon(QIcon(":/images/player_stop.png"));
    stopAction->setEnabled(false);
    connect(stopAction, SIGNAL(triggered()),
            this, SLOT(stop()));

    loopAction = new QAction(this);
    loopAction->setIcon(QIcon(":/images/player_loop.png"));
    loopAction->setEnabled(false);
    loopAction->setCheckable(true);
    connect(loopAction, SIGNAL(toggled(bool)),
            this, SLOT(loop(bool)));

    undoAction = new QAction(this);
    undoAction->setIcon(QIcon(":/images/undo.png"));
    undoAction->setEnabled(false);

    redoAction = new QAction(this);
    redoAction->setIcon(QIcon(":/images/redo.png"));
    redoAction->setEnabled(false);

    zoomInAction = new QAction(this);
    zoomInAction->setIcon(QIcon(":/images/zoomin.png"));

    zoomOutAction = new QAction(this);
    zoomOutAction->setIcon(QIcon(":/images/zoomout.png"));

    zoomFitAction = new QAction(this);
    zoomFitAction->setIcon(QIcon(":/images/zoomfit.png"));

    toggleMainToolbarAction = new QAction(this);
    toggleMainToolbarAction->setCheckable(true);
    toggleMainToolbarAction->setChecked(true);
    connect(toggleMainToolbarAction, SIGNAL(toggled(bool)),
            toolbar, SLOT(setVisible(bool)));

    toggleAnimationToolbarAction = new QAction(this);
    toggleAnimationToolbarAction->setCheckable(true);
    toggleAnimationToolbarAction->setChecked(true);
    connect(toggleAnimationToolbarAction, SIGNAL(toggled(bool)),
            animationBar, SLOT(setVisible(bool)));

    toggleScenarioDockAction = new QAction(this);
    toggleScenarioDockAction->setCheckable(true);
    toggleScenarioDockAction->setChecked(true);
    connect(toggleScenarioDockAction, SIGNAL(toggled(bool)),
            situationDock, SLOT(setVisible(bool)));

    aboutAction = new QAction(this);
    connect(aboutAction, SIGNAL(triggered()),
            this, SLOT(about()));
}

void MainWindow::updateActions() {
    SituationScene *scene = sceneList.at(currentSituation);

    bool selectedItems = !scene->selectedItems().isEmpty();
    bool selectedBoats = !scene->selectedBoatModels().isEmpty();
    bool selectedPoints = !scene->selectedPointModels().isEmpty();

    addBoatAction->setEnabled(selectedBoats || scene->state() == CREATE_BOAT);
    addPointAction->setEnabled(selectedPoints || scene->state() == CREATE_POINT);
    trimSailAction->setEnabled(selectedBoats);
    autotrimSailAction->setEnabled(selectedBoats);
    untrimSailAction->setEnabled(selectedBoats);
    togglePortOverlapAction->setEnabled(selectedBoats);
    toggleStarboardOverlapAction->setEnabled(selectedBoats);
    toggleHiddenAction->setEnabled(selectedBoats);
    toggleTextAction->setEnabled(selectedItems);
    flagMenu->setEnabled(selectedBoats);
    accelerationMenu->setEnabled(selectedBoats);
    deleteTrackAction->setEnabled(selectedBoats);
    deleteAction->setEnabled(selectedItems);

    bool allPortSet = 1;
    bool allStarboardSet = 1;
    bool allHiddenSet = 1;
    bool allKeelboat = 1;
    bool allSpinSet = 1;
    bool allTextSet = 1;
    bool allLaylinesSet = 1;
    int flagSize = ENUM_SIZE(Boats,Flag);
    bool allFlagSet[flagSize];
    for(int i=0; i < flagSize; i++) {
        allFlagSet[i] = 1;
    }
    int accelerationSize = ENUM_SIZE(Boats,Acceleration);
    bool allAccelerationSet[accelerationSize];
    for(int i=0; i<accelerationSize; i++) {
        allAccelerationSet[i] = 1;
    }
    foreach(BoatModel *boat, scene->selectedBoatModels()) {
        allPortSet = allPortSet && (boat->overlap() & Boats::port);
        allStarboardSet = allStarboardSet && (boat->overlap() & Boats::starboard);
        allHiddenSet = allHiddenSet && boat->hidden();
        allKeelboat = allKeelboat && (boat->track()->series() == Boats::keelboat);
        allSpinSet = allSpinSet && (boat->spin());
        for (int i = 0; i < flagSize; i++) {
            allFlagSet[i] = allFlagSet[i] && (boat->flag() == i);
        }
        for (int i=0; i<accelerationSize; i++) {
            allAccelerationSet[i] = allAccelerationSet[i] && (boat->acceleration() == i);
        }
    }
    togglePortOverlapAction->setChecked(selectedBoats && allPortSet);
    toggleStarboardOverlapAction->setChecked(selectedBoats && allStarboardSet);
    toggleHiddenAction->setChecked(selectedBoats && allHiddenSet);
    toggleSpinAction->setChecked(selectedBoats && allSpinSet);
    toggleSpinAction->setEnabled(selectedBoats && allKeelboat);

    foreach(PositionModel *position, scene->selectedModels()) {
        allTextSet = allTextSet && (!position->text().isEmpty());
        allLaylinesSet = allLaylinesSet && position->laylines();
    }
    toggleTextAction->setEnabled(scene->selectedModels().size()==1);
    toggleTextAction->setChecked(selectedItems && allTextSet);
    for (int i = 0; i < flagSize; i++) {
        QAction *flagAction = flagMenu->actions()[i];
        flagAction->setChecked(allFlagSet[i]);
    }
    for (int i=0; i<accelerationSize; i++) {
        QAction *accelerationAction = accelerationMenu->actions()[i];
        accelerationAction->setChecked(allAccelerationSet[i]);
    }
    toggleLaylinesAction->setChecked(selectedItems && allLaylinesSet);
    toggleLaylinesAction->setEnabled(selectedItems);
}

void MainWindow::changeState(SceneState newState) {
    SituationView *view = viewList.at(currentSituation);

    switch(newState) {
        case CREATE_TRACK:
            view->setCursor(Qt::CrossCursor);
            statusbar->showMessage(tr("Create Track"));
            addTrackAction->setChecked(true);
            addBoatAction->setChecked(false);
            addMarkAction->setChecked(false);
            addPolyLineAction->setChecked(false);
            addPointAction->setChecked(false);
            animateAction->setChecked(false);
            break;
        case CREATE_BOAT:
            view->setCursor(Qt::CrossCursor);
            statusbar->showMessage(tr("Create Boat"));
            addTrackAction->setChecked(false);
            addBoatAction->setChecked(true);
            addMarkAction->setChecked(false);
            addPolyLineAction->setChecked(false);
            addPointAction->setChecked(false);
            animateAction->setChecked(false);
            break;
        case CREATE_MARK:
            view->setCursor(Qt::CrossCursor);
            statusbar->showMessage(tr("Create Mark"));
            addTrackAction->setChecked(false);
            addBoatAction->setChecked(false);
            addMarkAction->setChecked(true);
            addPolyLineAction->setChecked(false);
            addPointAction->setChecked(false);
            animateAction->setChecked(false);
            break;
        case CREATE_LINE:
            view->setCursor(Qt::CrossCursor);
            statusbar->showMessage(tr("Create Line"));
            addTrackAction->setChecked(false);
            addBoatAction->setChecked(false);
            addMarkAction->setChecked(false);
            addPolyLineAction->setChecked(true);
            addPointAction->setChecked(false);
            animateAction->setChecked(false);
            break;
        case CREATE_POINT:
            view->setCursor(Qt::CrossCursor);
            statusbar->showMessage(tr("Create Line"));
            addTrackAction->setChecked(false);
            addBoatAction->setChecked(false);
            addMarkAction->setChecked(false);
            addPolyLineAction->setChecked(false);
            addPointAction->setChecked(true);
            animateAction->setChecked(false);
            break;
        case ANIMATE:
            statusbar->showMessage(tr("Animate"));
            addTrackAction->setChecked(false);
            addBoatAction->setChecked(false);
            addMarkAction->setChecked(false);
            addPolyLineAction->setChecked(false);
            addPointAction->setChecked(false);
            animateAction->setChecked(true);
            break;
        default:
            view->unsetCursor();
            statusbar->clearMessage();
            addTrackAction->setChecked(false);
            addBoatAction->setChecked(false);
            addMarkAction->setChecked(false);
            addPolyLineAction->setChecked(false);
            addPointAction->setChecked(false);
            animateAction->setChecked(false);
    }
    updateActions();
}

void MainWindow::cleanState(bool state) {
    SituationModel *situation = situationList.at(currentSituation);
    if (situation->fileName().isEmpty())
        saveFileAction->setEnabled(false);
    else
        saveFileAction->setEnabled(!state);
    saveAsAction->setEnabled(!state);
    QString shownName = QFileInfo(situation->fileName()).fileName();
    setWindowTitle(tr("%1 - %2 [*]").arg(tr("Boat Scenario")).arg(shownName));
    if (!state) {
        tabWidget->setTabText(currentSituation,shownName.append(" *"));
    } else {
        tabWidget->setTabText(currentSituation,shownName);
    }
    setWindowModified(!state);
}

void MainWindow::createMenus() {
    recentMenu = new QMenu(this);
    for (int i = 0; i < maxRecent; ++i) {
        QAction * recentAction = new QAction(this);
        recentMenu->addAction(recentAction);
        connect(recentAction, SIGNAL(triggered()),
                this, SLOT(openRecent()));
    }

    fileMenu = new QMenu(this);
    menubar->addMenu(fileMenu);
    fileMenu->addAction(newFileAction);
    fileMenu->addAction(openFileAction);
    fileMenu->addMenu(recentMenu);
    fileMenu->addAction(saveFileAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(printAction);
    fileMenu->addAction(printPreviewAction);
    fileMenu->addAction(exportPdfAction);
    fileMenu->addAction(exportImageAction);
#ifdef GIF_EXPORT
    fileMenu->addAction(exportAnimationAction);
#endif
    fileMenu->addSeparator();
    fileMenu->addAction(newTabAction);
    fileMenu->addAction(removeTabAction);
    fileMenu->addAction(restoreFilesAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    trackMenu = new QMenu(this);
    menubar->addMenu(trackMenu);
    trackMenu->addAction(addTrackAction);
    trackMenu->addAction(addBoatAction);
    trackMenu->addAction(addMarkAction);
    trackMenu->addAction(addPolyLineAction);
    trackMenu->addAction(addPointAction);
    trackMenu->addSeparator();
    trackMenu->addAction(trimSailAction);
    trackMenu->addAction(autotrimSailAction);
    trackMenu->addAction(untrimSailAction);
    trackMenu->addAction(togglePortOverlapAction);
    trackMenu->addAction(toggleStarboardOverlapAction);
    trackMenu->addAction(toggleHiddenAction);
    trackMenu->addAction(toggleTextAction);
    int flagSize = ENUM_SIZE(Boats, Flag);
    flagMenu = new QMenu(this);
    QActionGroup *flagGroup = new QActionGroup(flagMenu);
    for (int i = 0; i < flagSize; i++) {
        QAction * flagAction = new QAction(ENUM_NAME(Boats, Flag, i), this);
        flagAction->setCheckable(true);
        flagAction->setData(i);
        flagGroup->addAction(flagAction);
        flagMenu->addAction(flagAction);
        connect(flagAction, SIGNAL(triggered()),
                this, SLOT(toggleFlag()));
    }
    trackMenu->addMenu(flagMenu);
    int accelerationSize = ENUM_SIZE(Boats, Acceleration);
    accelerationMenu = new QMenu(this);
    QActionGroup *accelerationGroup = new QActionGroup(accelerationMenu);
    for (int i = 0; i < accelerationSize; i++) {
        QAction * accelerationAction = new QAction(ENUM_NAME(Boats, Acceleration, i), this);
        accelerationAction->setCheckable(true);
        accelerationAction->setData(i);
        accelerationGroup->addAction(accelerationAction);
        accelerationMenu->addAction(accelerationAction);
        connect(accelerationAction, SIGNAL(triggered()),
                this, SLOT(toggleAcceleration()));
    }
    trackMenu->addMenu(accelerationMenu);
    trackMenu->addAction(toggleSpinAction);
    trackMenu->addAction(toggleMarkZoneAction);
    trackMenu->addAction(toggleLaylinesAction);
    trackMenu->addSeparator();
    trackMenu->addAction(deleteTrackAction);
    trackMenu->addAction(deleteAction);

    defaultPopup = new QMenu(this);
    defaultPopup->addAction(addTrackAction);
    defaultPopup->addAction(addMarkAction);
    defaultPopup->addAction(addPolyLineAction);

    boatPopup = new QMenu(this);
    boatPopup->addAction(addBoatAction);
    boatPopup->addSeparator();
    boatPopup->addAction(trimSailAction);
    boatPopup->addAction(autotrimSailAction);
    boatPopup->addAction(untrimSailAction);
    boatPopup->addAction(togglePortOverlapAction);
    boatPopup->addAction(toggleStarboardOverlapAction);
    boatPopup->addAction(toggleHiddenAction);
    boatPopup->addAction(toggleTextAction);
    boatPopup->addMenu(flagMenu);
    boatPopup->addMenu(accelerationMenu);
    boatPopup->addAction(toggleSpinAction);
    boatPopup->addAction(toggleLaylinesAction);
    boatPopup->addSeparator();
    boatPopup->addAction(deleteTrackAction);
    boatPopup->addAction(deleteAction);

    markPopup = new QMenu(this);
    markPopup->addAction(toggleTextAction);
    markPopup->addAction(toggleMarkZoneAction);
    markPopup->addAction(toggleLaylinesAction);
    markPopup->addSeparator();
    markPopup->addAction(deleteAction);

    pointPopup = new QMenu(this);
    pointPopup->addAction(addPointAction);
    pointPopup->addSeparator();
    pointPopup->addAction(toggleTextAction);
    pointPopup->addAction(toggleLaylinesAction);
    pointPopup->addSeparator();
    pointPopup->addAction(deleteAction);

    historyMenu = new QMenu(this);
    menubar->addMenu(historyMenu);
    historyMenu->addAction(undoAction);
    historyMenu->addAction(redoAction);

    animationMenu = new QMenu(this);
    menubar->addMenu(animationMenu);
    animationMenu->addAction(animateAction);
    animationMenu->addSeparator();
    animationMenu->addAction(startAction);
    animationMenu->addAction(pauseAction);
    animationMenu->addAction(stopAction);
    animationMenu->addAction(loopAction);

    zoomMenu = new QMenu(this);
    menubar->addMenu(zoomMenu);
    zoomMenu->addAction(zoomInAction);
    zoomMenu->addAction(zoomFitAction);
    zoomMenu->addAction(zoomOutAction);

    viewMenu = new QMenu(this);
    menubar->addMenu(viewMenu);
    viewMenu->addAction(toggleMainToolbarAction);
    viewMenu->addAction(toggleAnimationToolbarAction);
    viewMenu->addAction(toggleScenarioDockAction);

    aboutMenu = new QMenu(this);
    menubar->addMenu(aboutMenu);
    langMenu = new QMenu(this);
    QActionGroup *langGroup = new QActionGroup(langMenu);
    QDir dir(TRANSLATEDIR);
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
                                          QDir::Name);
    fileNames.prepend("boats_en.qm");
    QMutableStringListIterator i(fileNames);
    while (i.hasNext()) {
        i.next();
        QRegExp rx("boats_(.+)\\.qm");
        rx.indexIn(i.value());
        QLocale locale(rx.cap(1));
        QAction *langAction = new QAction(
                QLocale::languageToString(locale.language()), this);
        langAction->setCheckable(true);
        langAction->setData(locale.name());
        langGroup->addAction(langAction);
        langMenu->addAction(langAction);
        connect(langAction, SIGNAL(triggered()),
                this, SLOT(toggleLang()));
    }
    aboutMenu->addMenu(langMenu);
    aboutMenu->addAction(aboutAction);

    toolbar->addAction(newFileAction);
    toolbar->addAction(openFileAction);
    toolbar->addAction(saveFileAction);
    toolbar->addAction(saveAsAction);
    toolbar->addAction(exportImageAction);
#ifdef GIF_EXPORT
    toolbar->addAction(exportAnimationAction);
#endif
    toolbar->addSeparator();
    toolbar->addAction(undoAction);
    toolbar->addAction(redoAction);
    toolbar->addSeparator();
    toolbar->addAction(addTrackAction);
    toolbar->addAction(addBoatAction);
    toolbar->addAction(addMarkAction);
    toolbar->addAction(addPolyLineAction);
    toolbar->addAction(addPointAction);
    toolbar->addSeparator();
    toolbar->addAction(animateAction);
    toolbar->addSeparator();
    toolbar->addAction(zoomOutAction);
    toolbar->addAction(zoomFitAction);
    toolbar->addAction(zoomInAction);


    animationSlider = new QSlider(Qt::Horizontal, this);
    animationSlider->setTickInterval(2000);
    animationSlider->setTickPosition(QSlider::TicksBelow);
    animationSlider->setSingleStep(400);
    animationSlider->setPageStep(2000);
    animationSlider->setEnabled(false);

    animationBar->addAction(startAction);
    animationBar->addAction(pauseAction);
    animationBar->addAction(stopAction);
    animationBar->addAction(loopAction);
    animationBar->addSeparator();
    animationBar->addWidget(animationSlider);
}

void MainWindow::createDocks() {
    QScrollArea *area = new QScrollArea(this);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidgetResizable(true);
    area->setWidget(situationWidget);
    situationDock->setWidget(area);
    situationDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void MainWindow::newTab() {
    SituationModel *situation = new SituationModel(this);
    SituationScene *scene = new SituationScene(situation);
    SituationView *view =new SituationView(scene);

    situationList.append(situation);
    sceneList.append(scene);
    scene->setDefaultPopup(defaultPopup);
    scene->setBoatPopup(boatPopup);
    scene->setMarkPopup(markPopup);
    scene->setPointPopup(pointPopup);
    viewList.append(view);
    tabWidget->addTab(view, "");

    tabWidget->setCurrentIndex(situationList.size()-1);
    view->setFocus();
    view->setMouseTracking(true);
    if (situationList.size() > 1) {
        removeTabAction->setEnabled(true);
        removeTabButton->setEnabled(true);
    }
}

void MainWindow::unsetTab() {
    if (situationList.size() == 1) {
        return;
    }

    std::cout << "unsetTab" << std::endl;
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);
    if (scene->state() == ANIMATE) {
        scene->setState(NO_STATE);
    }

    disconnect(situation->undoStack(), 0, 0, 0);
    disconnect(scene, 0, this, 0);

    disconnect(undoAction, 0, 0, 0);
    disconnect(redoAction, 0, 0, 0);

    disconnect(zoomInAction, 0, 0, 0);
    disconnect(zoomOutAction, 0, 0, 0);
    disconnect(zoomFitAction, 0, 0, 0);

    situationWidget->unSetSituation();
}

void MainWindow::setTab(int index) {
    std::cout << "setTab " << index << std::endl;
    unsetTab();
    currentSituation = index;
    SituationModel *situation = situationList.at(index);
    SituationScene *scene = sceneList.at(index);
    SituationView *view = viewList.at(index);

    connect(undoAction, SIGNAL(triggered()),
            situation->undoStack(), SLOT(undo()));
    connect(situation->undoStack(), SIGNAL(canUndoChanged(bool)),
            undoAction, SLOT(setEnabled(bool)));
    undoAction->setEnabled(situation->undoStack()->canUndo()),

    connect(redoAction, SIGNAL(triggered()),
            situation->undoStack(), SLOT(redo()));
    connect(situation->undoStack(), SIGNAL(canRedoChanged(bool)),
            redoAction, SLOT(setEnabled(bool)));
    redoAction->setEnabled(situation->undoStack()->canRedo());

    connect(situation->undoStack(), SIGNAL(cleanChanged(bool)),
            this, SLOT(cleanState(bool)));
    cleanState(situation->undoStack()->isClean());

    connect(zoomInAction, SIGNAL(triggered()),
            view, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered()),
            view, SLOT(zoomOut()));
    connect(zoomFitAction, SIGNAL(triggered()),
            view, SLOT(zoomFit()));

    situationWidget->setSituation(situation);
    connect(scene, SIGNAL(stateChanged(SceneState)),
            this, SLOT(changeState(SceneState)));
    changeState(scene->state());

    connect(scene, SIGNAL(selectedModelsChanged()),
            this, SLOT(updateActions()));
    updateActions();
}

void MainWindow::removeTab() {
    int index = tabWidget->currentIndex();
    SituationModel *situation = situationList.at(index);
    SituationScene *scene = sceneList.at(index);
    SituationView *view = viewList.at(index);

    if (!maybeSave(situation)) {
        return;
    }

    if (index == situationList.size() - 1) {
        tabWidget->setCurrentIndex(index - 1);
    } else if (index != situationList.size() - 2) {
        tabWidget->setCurrentIndex(index + 1);
    }
    situationList.removeAt(index);
    sceneList.removeAt(index);
    viewList.removeAt(index);

    tabWidget->removeTab(index);
    view->deleteLater();
    scene->deleteLater();
    situation->deleteLater();
    if (situationList.size() == 1) {
        removeTabAction->setEnabled(false);
        removeTabButton->setEnabled(false);
    }
}

void MainWindow::writeSettings() {
    QSettings settings("Boats");

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("AnimationBar", animationBar->isVisible());
    settings.setValue("AnimationBarArea", toolBarArea(animationBar));
    settings.setValue("ToolBar", toolbar->isVisible());
    settings.setValue("ToolBarArea", toolBarArea(toolbar));
    settings.setValue("ScenarioDock", situationDock->isVisible());
    QStringList fileList;
    foreach(SituationModel *situation, situationList) {
        if (!situation->fileName().isEmpty()) {
            fileList.append(situation->fileName());
        }
    }
    settings.setValue("recentList", recentList);
    settings.setValue("fileList",fileList);
    settings.setValue("filePath", filePath);
    settings.endGroup();
}

void MainWindow::readSettings() {
    QSettings settings("Boats");

    settings.beginGroup("MainWindow");
    QSize size = settings.value("size").toSize();
    QPoint pos = settings.value("pos").toPoint();
    if ( (size.isValid()) && (!pos.isNull()) ) {
        resize(size);
        move(pos);
    } else {
        showMaximized();
    }
    toggleAnimationToolbarAction->setChecked(settings.value("AnimationBar", true).toBool());
    addToolBar((Qt::ToolBarArea)(settings.value("AnimationBarArea").toInt()), animationBar);
    toggleMainToolbarAction->setChecked(settings.value("ToolBar", true).toBool());
    addToolBar((Qt::ToolBarArea)(settings.value("ToolBarArea").toInt()), toolbar);
    toggleScenarioDockAction->setChecked(settings.value("ScenarioDock", true).toBool());

    recentList = settings.value("recentList").toStringList();
    updateRecentList();

    fileList = settings.value("fileList").toStringList();
    filePath = settings.value("filePath").toString();
    settings.endGroup();
}

void MainWindow::updateRecentList() {
    int numRecentFiles = qMin(recentList.size(), maxRecent);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(recentList[i]).fileName());
        QAction * recentAction = recentMenu->actions()[i];
        recentAction->setText(text);
        recentAction->setData(i);
        recentAction->setVisible(true);
    }
    for (int j = numRecentFiles; j < maxRecent; ++j) {
        QAction * recentAction = recentMenu->actions()[j];
        recentAction->setVisible(false);
    }
}

bool MainWindow::maybeSave(SituationModel *situation) {
    if (!situation->undoStack()->isClean()) {
        QString shownName = QFileInfo(situation->fileName()).fileName();
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, shownName,
                    tr("The document %1 has been modified.\n"
                        "Do you want to save your changes?").arg(shownName),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return saveSituation(situation, "");
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    SituationScene *scene = sceneList.at(currentSituation);

    bool animated = (scene->state() == ANIMATE);
    if (animated) {
        scene->setState(NO_STATE);
    }
    foreach(SituationModel *situation, situationList) {
        if (!maybeSave(situation)) {
            event->ignore();
            return;
        }
    }
    writeSettings();
    event->accept();
}

void MainWindow::changeEvent(QEvent *event) {
    if(event->type() == QEvent::LanguageChange) {
        newFileAction->setText(tr("&New File"));
        newFileAction->setShortcut(tr("Ctrl+N"));

        openFileAction->setText(tr("&Open File..."));
        openFileAction->setShortcut(tr("Ctrl+O"));

        saveFileAction->setText(tr("&Save File"));
        saveFileAction->setShortcut(tr("Ctrl+S"));

        saveAsAction->setText(tr("Save &As..."));
        saveAsAction->setShortcut(tr("Ctrl+Shift+S"));

        newTabAction->setText(tr("New &Tab"));
        newTabAction->setShortcut(tr("Ctrl+T"));

        removeTabAction->setText(tr("&Close Tab"));
        removeTabAction->setShortcut(tr("Ctrl+W"));

        restoreFilesAction->setText(tr("&Restore Last Session..."));

        printAction->setText(tr("&Print..."));
        printAction->setShortcut(tr("Ctrl+P"));

        printPreviewAction->setText(tr("Print P&review..."));
        printPreviewAction->setShortcut(tr("Ctrl+R"));

        exportPdfAction->setText(tr("&Export Pdf..."));
        exportPdfAction->setShortcut(tr("Ctrl+E"));

        exportImageAction->setText(tr("Export &Image..."));
        exportImageAction->setShortcut(tr("Ctrl+I"));

#ifdef GIF_EXPORT
        exportAnimationAction->setText(tr("Export Ani&mation..."));
        exportAnimationAction->setShortcut(tr("Ctrl+V"));
#endif

        exitAction->setText(tr("E&xit"));
        exitAction->setShortcut(tr("Ctrl+Q"));
        exitAction->setStatusTip(tr("Exit the application"));

        addTrackAction->setText(tr("Create &Track"));
        addTrackAction->setShortcut(tr("Ctrl+Ins"));

        addBoatAction->setText(tr("Create &Boat"));
        addBoatAction->setShortcut(tr("Ins"));

        addMarkAction->setText(tr("Create &Mark"));
        addMarkAction->setShortcut(tr("Alt+Ins"));

        addPolyLineAction->setText(tr("Create &PolyLine"));
        addPolyLineAction->setShortcut(tr("Ctrl+Alt+Ins"));

        addPointAction->setText(tr("Create Poin&t"));
        addPointAction->setShortcut(tr("Ctrl+T"));

        trimSailAction->setText(tr("Trim Sail"));
        trimSailAction->setShortcut(tr("<"));

        autotrimSailAction->setText(tr("Auto Trim"));
        autotrimSailAction->setShortcut(tr("="));

        untrimSailAction->setText(tr("Untrim Sail"));
        untrimSailAction->setShortcut(tr(">"));

        togglePortOverlapAction->setText(tr("&Port Overlap"));
        togglePortOverlapAction->setShortcut(tr("Alt+<"));

        toggleStarboardOverlapAction->setText(tr("&Starboard Overlap"));
        toggleStarboardOverlapAction->setShortcut(tr("Alt+>"));

        toggleHiddenAction->setText(tr("&Hide"));
        toggleHiddenAction->setShortcut(tr("Alt+D"));

        toggleTextAction->setText(tr("&Text"));
        toggleTextAction->setShortcut(tr("Alt+T"));

        toggleSpinAction->setText(tr("Toggle &Spinnaker"));
        toggleSpinAction->setShortcut(tr("Alt+S"));

        toggleMarkZoneAction->setText(tr("Toggle Mark &Zone"));
        toggleMarkZoneAction->setShortcut(tr("Alt+Z"));

        toggleLaylinesAction->setText(tr("Toggle &Laylines"));
        toggleLaylinesAction->setShortcut(tr("Alt+L"));

        deleteTrackAction->setText(tr("Delete Track"));
        deleteTrackAction->setShortcut(tr("Ctrl+Del"));

        deleteAction->setText(tr("&Delete Selection"));
        deleteAction->setShortcut(tr("Del"));

        animateAction->setText(tr("&Animate"));
        animateAction->setShortcut(tr("Ctrl+A"));

        startAction->setText(tr("&Play"));
        startAction->setShortcut(tr("P"));

        pauseAction->setText(tr("&Pause"));
        pauseAction->setShortcut(tr("M"));

        stopAction->setText(tr("&Stop"));
        stopAction->setShortcut(tr("Space"));

        loopAction->setText(tr("&Loop"));
        loopAction->setShortcut(tr("L"));

        undoAction->setText(tr("&Undo"));
        undoAction->setShortcut(tr("Ctrl+Z"));

        redoAction->setText(tr("&Redo"));
        QList<QKeySequence> redoShortcuts;
        redoShortcuts << tr("Ctrl+Y") << tr("Shift+Ctrl+Z");
        redoAction->setShortcuts(redoShortcuts);

        zoomInAction->setText(tr("Zoom &In"));
        zoomInAction->setShortcut(tr("Ctrl++"));

        zoomOutAction->setText(tr("Zoom &Out"));
        zoomOutAction->setShortcut(tr("Ctrl+-"));

        zoomFitAction->setText(tr("Zoom &Fit"));
        zoomFitAction->setShortcut(tr("Ctrl+F"));

        toggleMainToolbarAction->setText(tr("Main Toolbar"));
        toggleAnimationToolbarAction->setText(tr("Animation Toolbar"));
        toggleScenarioDockAction->setText(tr("Scenario Dock"));

        aboutAction->setText(tr("&About"));

        recentMenu->setTitle(tr("&Recent"));
        fileMenu->setTitle(tr("&File"));
        trackMenu->setTitle(tr("&Edit"));
        flagMenu->setTitle(tr("&Flag"));
        accelerationMenu->setTitle(tr("&Acceleration"));
        historyMenu->setTitle(tr("&History"));
        animationMenu->setTitle(tr("&Animation"));
        zoomMenu->setTitle(tr("&Zoom"));
        viewMenu->setTitle(tr("&View"));
        aboutMenu->setTitle(tr("&Help"));
        langMenu->setTitle(tr("Choose &Language"));
    } else {
        QMainWindow::changeEvent(event);
    }
}

void MainWindow::newFile() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);
    SituationView *view = viewList.at(currentSituation);

    if (maybeSave(situation)) {
        scene->setState(NO_STATE);
        situation->undoStack()->setIndex(0);
        setCurrentFile(situation, "");
        situation->undoStack()->clear();
        situation->wind().clearWind();
        situationWidget->unSetSituation();
        situationWidget->setSituation(situation);
        view->centerOn(0,0);
        view->resetMatrix();
    }
}

void MainWindow::openFile() {
    QString fileName =
             QFileDialog::getOpenFileName(this, tr("Open Scenario File"),
                                          filePath,
                                          tr("xmlscenario Files (*.xbs)"));
    if (fileName.isEmpty())
        return;

    filePath = QFileInfo(fileName).absolutePath();
    openFile(fileName);
}

void MainWindow::openRecent() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int item = action->data().toInt();
        if (recentList.size() > item) {
            openFile(recentList[item]);
        }
    }
}

void MainWindow::restoreFiles() {
    openFiles(fileList);
}

void MainWindow::openFiles(QStringList fileList) {
    foreach (const QString fileName, fileList) {
        std::cout << "opening " << fileName.toStdString() << std::endl;
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            openFile(fileName, fileList.first() != fileName);
        }
    }
}

void MainWindow::openFile(const QString &fileName, bool inNewTab) {
    if (inNewTab) {
        // create new tab
        newTab();
        tabWidget->setCurrentIndex(situationList.size() - 1);
    } else {
        // delete situation;
        newFile();
    }

    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);
    SituationView *view = viewList.at(currentSituation);

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Open Scenario File"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    XmlSituationReader reader(situation);
    if (!reader.read(&file)) {
        QMessageBox::warning(this, tr("Open Scenario file"),
                             tr("Parse error in file %1 at line %2, column %3:\n%4")
                             .arg(fileName)
                             .arg(reader.lineNumber())
                             .arg(reader.columnNumber())
                             .arg(reader.errorString()));
        return;
    }
    situationWidget->update();
    setCurrentFile(situation, fileName);
    view->centerOn(scene->itemsBoundingRect().center());
    statusbar->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveSituation(SituationModel *situation, QString fileName) {
    QString name = fileName;
    if (name.isEmpty()) {
        QString defaultFile;
        if (situation->fileName().isEmpty()) {
            defaultFile = QDateTime::currentDateTime().toString("yyMMdd") + ".xbs";
        } else {
            defaultFile = situation->fileName();
        }
        name = QFileDialog::getSaveFileName(this, tr("Save Scenario"),
                                            defaultFile,
                                            tr("xmlscenario Files (*.xbs)"),
                                            NULL,
                                            QFileDialog::DontConfirmOverwrite);
        if (name.isEmpty()) {
            return false;
        }
        if (QFileInfo(name).suffix().isEmpty())
            name.append(".xbs");
        if (QFile::exists(name)) {
            if (QMessageBox::warning(this, tr("Save Scenario"),
                                     tr("%1 already exists.\nDo you want to replace it?")
                                     .arg(QFileInfo(name).baseName()),
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
                    == QMessageBox::No) {
                return false;
            }
        }
    }

    QFile file(name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Save Scenario"),
                            tr("Cannot write file %1:\n%2.")
                            .arg(fileName)
                            .arg(file.errorString()));
        return false;
    }
    XmlSituationWriter writer(situation);
    writer.writeFile(&file);
    setCurrentFile(situation, name);
    statusbar->showMessage(tr("File saved"), 2000);
    return true;
}

bool MainWindow::saveFile() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    bool animated = (scene->state() == ANIMATE);
    if (animated) {
        scene->setState(NO_STATE);
    }
    bool saved = saveSituation(situation, situation->fileName());
    if (animated) {
        animate(true);
    }
    return saved;
}

bool MainWindow::saveAs() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    bool animated = (scene->state() == ANIMATE);
    if (animated) {
        scene->setState(NO_STATE);
    }
    bool saved = saveSituation(situation, "");
    if (animated) {
        animate(true);
    }
    return saved;
}

void MainWindow::print() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationView *view = viewList.at(currentSituation);
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted) {
        return;
    }

    SituationPrint printSituation(situation, view);
    printSituation.render(printer.paperRect(QPrinter::Millimeter).adjusted(20, 20, -20, -20));
    printSituation.print(&printer);
}

void MainWindow::printPreview() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationView *view = viewList.at(currentSituation);
    SituationPrint printSituation(situation, view);
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog dialog(&printer);
    printSituation.render(printer.paperRect(QPrinter::Millimeter).adjusted(20, 20, -20, -20));

    connect(&dialog, SIGNAL(paintRequested(QPrinter*)),
            &printSituation, SLOT(print(QPrinter*)));
    dialog.exec();
}

void MainWindow::exportPdf() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationView *view = viewList.at(currentSituation);

    QString defaultName(situation->fileName());
    defaultName.chop(4);
    QString fileName = QFileDialog::getSaveFileName(this, "Export to PDF",
                                                    defaultName, "PDF Files (*.pdf)");
    if (fileName.isEmpty()) {
        return;
    }
    if (QFileInfo(fileName).suffix().isEmpty())
        fileName.append(".pdf");
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    SituationPrint printSituation(situation, view);
    printSituation.render(printer.paperRect(QPrinter::Millimeter).adjusted(20, 20, -20, -20));
    printSituation.print(&printer);
}

void MainWindow::exportImage() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationView *view = viewList.at(currentSituation);

    QString defaultName(situation->fileName());
    defaultName.chop(4);
    QList<QByteArray> formatsList = QImageWriter::supportedImageFormats();
#if defined(Q_WS_MAC)
    QString formats("PNG Image Files (*.png)");
#else
    QString formats;
    for (int i=0; i<formatsList.size(); i++)
	formats.append(formatsList[i].toUpper()).append(" Files (*.").append(formatsList[i]).append(");;");
#endif
    QString ext;
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save Scenario"),
                                         defaultName,
                                         formats,
                                         &ext);
    if (fileName.isEmpty()) {
        return;
    }

    // if no provided extension or incorrect extension, use selected filter
    if (!formatsList.contains(QFileInfo(fileName).suffix().toAscii())) {
#if defined(Q_WS_MAC)
	fileName.append(".png");
#else
	fileName.append(".").append(ext.left(ext.indexOf(' ')).toLower());
#endif
    }

    QPixmap pixmap = view->screenShot();
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Export Image"),
                            tr("Cannot write file %1:\n%2.")
                            .arg(fileName)
                            .arg(file.errorString()));
        return;
    }
    pixmap.save(fileName);
}

#ifdef GIF_EXPORT
void MainWindow::exportAnimation() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationView *view = viewList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QString defaultName(situation->fileName());
    defaultName.chop(4);
    QString format("GIF Files (*.gif)");
    QString ext;
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Export Animation"),
                                         defaultName,
                                         format,
                                         &ext);
    if (fileName.isEmpty()) {
        return;
    }

    // if no provided extension or incorrect extension, use selected filter
    int dotIndex = fileName.lastIndexOf(".");
    QString baseName = fileName.left(dotIndex);
    QString newExt = fileName.right(fileName.size()-dotIndex-1);
    if (!newExt.toAscii().contains("gif")) {
        fileName.append(".gif");
    }

    GifWriter *writer = new GifWriter();

    animate(true, false);
    QProgressDialog progress(tr("Exporting Animation..."), tr("Abort"), 0, scene->animation()->duration(), this);
    progress.setWindowModality(Qt::WindowModal);
    statusbar->showMessage("Exporting animation");
    scene->animation()->setCurrentTime(scene->animation()->duration()/2);
    QPixmap pixmap = view->screenShot();
    QImage shot = pixmap.toImage().convertToFormat(QImage::Format_Indexed8);
    writer->setColorMap(shot);

    QList<QImage*> imageList;
    for (int i=0; i<=scene->animation()->duration(); i+=80) {
        scene->animation()->setCurrentTime(i);
        pixmap = view->screenShot();
        QImage *image = new QImage(pixmap.toImage()
                                   .convertToFormat(QImage::Format_Indexed8, writer->colormap()));
        imageList.append(image);
        progress.setValue(i);
        if (progress.wasCanceled()) {
            break;
        }
    }
    if (!progress.wasCanceled()) {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly)) {
            QMessageBox::warning(this, tr("Export Animation"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
            return;
        }
        writer->setDevice(&file);
        writer->write(imageList);
    }
    animate(false);
}
#endif

void MainWindow::setCurrentFile(SituationModel *situation, const QString &fileName) {
    situation->setFileName(fileName);
    situation->undoStack()->setClean();

    if (!fileName.isEmpty()) {
        recentList.removeAll(fileName);
        recentList.prepend(fileName);
        while (recentList.size() > maxRecent)
            recentList.removeLast();
        updateRecentList();
    }
}

void MainWindow::addTrack() {
    SituationScene *scene = sceneList.at(currentSituation);

    if(scene->state() == CREATE_TRACK) {
        scene->setState(NO_STATE);
    } else {
        scene->setState(CREATE_TRACK);
    }
}

void MainWindow::deleteTrack() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    // TODO trick to delete first selected track
    if (!scene->selectedBoatModels().isEmpty()) {
        BoatModel *boat = scene->selectedBoatModels()[0];
        TrackModel * track = boat->track();
        situation->undoStack()->push(new DeleteTrackUndoCommand(situation, track));
    }
}

void MainWindow::addBoat() {
    SituationScene *scene = sceneList.at(currentSituation);

    if (scene->state() == CREATE_BOAT) {
        scene->setState(NO_STATE);
    } else {
        scene->setState(CREATE_BOAT);
    }
}

void MainWindow::deleteModels() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    foreach (BoatModel *boat, scene->selectedBoatModels()) {
        TrackModel* track = boat->track();
        if (track->size() > 1) {
            situation->undoStack()->push(new DeleteBoatUndoCommand(track, boat));
        } else {
            situation->undoStack()->push(new DeleteTrackUndoCommand(situation, track));
        }
    }
    foreach (MarkModel *mark, scene->selectedMarkModels()) {
        situation->undoStack()->push(new DeleteMarkUndoCommand(situation, mark));
    }

    foreach (PointModel *point, scene->selectedPointModels()) {
        PolyLineModel *polyLine = point->polyLine();
        if (polyLine->size() > 1) {
            situation->undoStack()->push(new DeletePointUndoCommand(polyLine, point));
        } else {
            situation->undoStack()->push(new DeletePolyLineUndoCommand(situation, polyLine));
        }
    }
}

void MainWindow::addMark() {
    SituationScene *scene = sceneList.at(currentSituation);

    if (scene->state() == CREATE_MARK) {
        scene->setState(NO_STATE);
    } else {
        scene->setState(CREATE_MARK);
    }
}

void MainWindow::addPolyLine() {
    SituationScene *scene = sceneList.at(currentSituation);

    if (scene->state() == CREATE_LINE) {
        scene->setState(NO_STATE);
    } else {
        scene->setState(CREATE_LINE);
    }
}

void MainWindow::addPoint() {
    SituationScene *scene = sceneList.at(currentSituation);

    if (scene->state() == CREATE_POINT) {
        scene->setState(NO_STATE);
    } else {
        scene->setState(CREATE_POINT);
    }
}

void MainWindow::trimSail() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<BoatModel *> boatList = scene->selectedBoatModels();
    if (! boatList.isEmpty()) {
        qreal trim = boatList[0]->trim();
        if (boatList[0]->heading() < 180) {
            trim -= 5;
        } else {
            trim += 5;
        }
        situation->undoStack()->push(new TrimBoatUndoCommand(boatList, trim));
    }
}

void MainWindow::autotrimSail() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<BoatModel *> boatList = scene->selectedBoatModels();
    if (! boatList.isEmpty()) {
        situation->undoStack()->push(new TrimBoatUndoCommand(boatList, 0));
    }
}

void MainWindow::untrimSail() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<BoatModel *> boatList = scene->selectedBoatModels();
    if (! boatList.isEmpty()) {
        qreal trim = boatList[0]->trim();
        if (boatList[0]->heading() < 180) {
            trim += 5;
        } else {
            trim -= 5;
        }
        situation->undoStack()->push(new TrimBoatUndoCommand(boatList, trim));
    }
}

void MainWindow::togglePortOverlap() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<BoatModel *> boatList = scene->selectedBoatModels();
    if (! boatList.isEmpty()) {
        situation->undoStack()->push(new OverlapBoatUndoCommand(situation, boatList, Boats::port));
    }
}

void MainWindow::toggleStarboardOverlap() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<BoatModel *> boatList = scene->selectedBoatModels();
    if (! boatList.isEmpty()) {
        situation->undoStack()->push(new OverlapBoatUndoCommand(situation, boatList, Boats::starboard));
    }
}

void MainWindow::toggleHidden() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<BoatModel *> boatList = scene->selectedBoatModels();
    if (! boatList.isEmpty()) {
        situation->undoStack()->push(new HiddenBoatUndoCommand(situation, boatList, !boatList.first()->hidden()));
    }
}

void MainWindow::toggleText() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<PositionModel *> modelList = scene->selectedModels();
    if (! modelList.isEmpty()) {
        QString text;
        if (modelList.first()->text().isEmpty()) {
            text = tr("Protest!");
        }
        situation->undoStack()->push(new SetTextUndoCommand(modelList.first(), text));
    }
}

void MainWindow::toggleFlag() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        Boats::Flag flag = (Boats::Flag)action->data().toInt();

        QList<BoatModel *> boatList = scene->selectedBoatModels();
        if (! boatList.isEmpty()) {
            situation->undoStack()->push(new FlagBoatUndoCommand(situation, boatList, flag));
        }
    }
}

void MainWindow::toggleAcceleration() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        Boats::Acceleration acceleration = (Boats::Acceleration)action->data().toInt();

        QList<BoatModel *> boatList = scene->selectedBoatModels();
        if (! boatList.isEmpty()) {
            situation->undoStack()->push(new AccelerateBoatUndoCommand(situation, boatList, acceleration));
        }
    }
}

void MainWindow::toggleSpin() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<BoatModel *> boatList = scene->selectedBoatModels();
    if (! boatList.isEmpty()) {
        situation->undoStack()->push(new SpinBoatUndoCommand(situation, boatList, !boatList.first()->spin()));
    }
}

void MainWindow::toggleMarkZone() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<MarkModel *> markList = scene->selectedMarkModels();
    if (! markList.isEmpty()) {
        situation->undoStack()->push(new ZoneMarkUndoCommand(situation, markList));
    } else {
        situation->undoStack()->push(new ZoneMarkUndoCommand(situation, situation->marks()));
    }
}

void MainWindow::toggleLaylines() {
    SituationModel *situation = situationList.at(currentSituation);
    SituationScene *scene = sceneList.at(currentSituation);

    QList<PositionModel *> modelList = scene->selectedModels();
    if (! modelList.isEmpty()) {
        situation->undoStack()->push(new SetLaylinesUndoCommand(modelList, !modelList.first()->laylines()));
    }
}

void MainWindow::toggleLang() {

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {

        qApp->removeTranslator(qtTranslator);
        qApp->removeTranslator(translator);

        createTranslations(action->data().toString());
    }
}

void MainWindow::animate(bool state, bool interactive) {
    SituationScene *scene = sceneList.at(currentSituation);

    if (state) {
        if (scene->state() != ANIMATE) {
            scene->setState(ANIMATE);
            scene->setAnimation();

            connect(scene->animation(), SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)),
                    this, SLOT(changeAnimationState(QAbstractAnimation::State, QAbstractAnimation::State)));
            connect(scene->animation(), SIGNAL(timeChanged(int)),
                    animationSlider, SLOT(setValue(int)));
            connect(animationSlider, SIGNAL(valueChanged(int)),
                    scene->animation(), SLOT(setCurrentTime(int)));

            scene->animation()->setCurrentTime(0);
            if (interactive) {
                if (!toggleAnimationToolbarAction->isChecked()) {
                    toggleAnimationToolbarAction->setChecked(true);
                }
                animationSlider->setRange(0,scene->animation()->duration());
                animationSlider->setEnabled(true);
                startAction->setEnabled(true);
                loopAction->setEnabled(true);
                }
            }
    } else {
        if (scene->state() == ANIMATE) {
            scene->setState(NO_STATE);
        }
        scene->unSetAnimation();
        disconnect(this, SLOT(changeAnimationState(QAbstractAnimation::State,QAbstractAnimation::State)));
        disconnect(animationSlider, SLOT(setValue(int)));
        disconnect(scene->animation(), SLOT(setCurrentTime(int)));

        animationSlider->setEnabled(false);
        scene->animation()->stop();
        startAction->setEnabled(false);
        stopAction->setEnabled(false);
        loopAction->setEnabled(false);
    }
}

void MainWindow::play() {
    if (debugLevel & 1 << ANIMATION) std::cout << "playing" << std::endl;
    SituationScene *scene = sceneList.at(currentSituation);
    pauseAction->setChecked(false);
    scene->animation()->start();
}

void MainWindow::pause(bool pause) {
    SituationScene *scene = sceneList.at(currentSituation);
    if (pause) {
        if (debugLevel & 1 << ANIMATION) std::cout << "pausing" << std::endl;
        scene->animation()->setPaused(true);
    } else {
        if (debugLevel & 1 << ANIMATION) std::cout << "resuming" << std::endl;
        scene->animation()->setPaused(false);
    }
}

void MainWindow::stop() {
    if (debugLevel & 1 << ANIMATION) std::cout << "stopping" << std::endl;
    SituationScene *scene = sceneList.at(currentSituation);
    pauseAction->setChecked(false);
    scene->animation()->stop();
    scene->animation()->setCurrentTime(0);
}

void MainWindow::loop(bool loop) {
    SituationScene *scene = sceneList.at(currentSituation);
    if (loop) {
        if (debugLevel & 1 << ANIMATION) std::cout << "loop play" << std::endl;
        scene->animation()->setLoopCount(0);
    } else {
        if (debugLevel & 1 << ANIMATION) std::cout << "single play" << std::endl;
        scene->animation()->setLoopCount(1);
    }
}

void MainWindow::changeAnimationState(QAbstractAnimation::State newState, QAbstractAnimation::State) {
    switch(newState) {
        case QAbstractAnimation::Running:
            if (debugLevel & 1 << ANIMATION) std::cout << "state running" << std::endl;
            startAction->setEnabled(false);
            pauseAction->setEnabled(true);
            stopAction->setEnabled(true);
            animationSlider->blockSignals(true);
            break;

        case QAbstractAnimation::Paused:
            if (debugLevel & 1 << ANIMATION) std::cout << "state paused" << std::endl;
            startAction->setEnabled(true);
            pauseAction->setEnabled(true);
            stopAction->setEnabled(true);
            animationSlider->blockSignals(false);
            break;

        case QAbstractAnimation::Stopped:
            if (debugLevel & 1 << ANIMATION) std::cout << "state not running" << std::endl;
            startAction->setEnabled(true);
            pauseAction->setEnabled(false);
            stopAction->setEnabled(false);
            animationSlider->blockSignals(false);
            break;
    }
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Boat Scenario"),
	QString("<center><img src=\":/images/about.png\">"
            "<p><b>Boat Scenario</b> - a Race Scenario drawing tool.</p>"
            "<p>Version %1</p></center>"
            "<p>Copyright (C) 2008-2011 Thibaut GRIDEL </p>"
            "<p></p>"
            "<p>visit <a href=\"http://boats.sf.net\">http://boats.sf.net</a></p>"
            "<p>This program is free software; you can redistribute it and/or modify "
            "it under the terms of the GNU General Public License as published by "
            "the Free Software Foundation; either version 3 of the License, or "
            "(at your option) any later version.</p>"
            "<p></p>"
            "<p>This program is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
            "GNU General Public License for more details.</p>").arg(VERSION));
}
