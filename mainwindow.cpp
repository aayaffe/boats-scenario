//
// C++ Implementation: mainwindow
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2019 Thibaut GRIDEL
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
#include "mainwindow.h"

#include "commontypes.h"
#include "boats.h"

#include "situationmodel.h"
#include "trackmodel.h"
#include "boatmodel.h"
#include "markmodel.h"
#include "polylinemodel.h"
#include "pointmodel.h"

#include "statemachine.h"
#include "scenarioanimation.h"

#include "xmlsituationreader.h"
#include "xmlsituationwriter.h"

#include "situationwidget.h"
#include "situationscene.h"
#include "situationview.h"
#include "colorpickerwidget.h"

#ifdef GIF_EXPORT
#include "gifwriter.h"
#endif

#include "boatsengine.h"

#include <QMenuBar>
#include <QToolBar>
#include <QTabWidget>
#include <QDockWidget>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include <iostream>

#define VERSION "201908"

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
        translator(new QTranslator(this)),
        engine(new BoatsEngine()) {

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
            engine, SLOT(addTrack()));

    addBoatAction = new QAction(this);
    addBoatAction->setIcon(QIcon(":/images/addboat.png"));
    addBoatAction->setCheckable(true);
    connect(addBoatAction, SIGNAL(triggered()),
            engine, SLOT(addBoat()));

    addMarkAction = new QAction(this);
    addMarkAction->setIcon(QIcon(":/images/addmark.png"));
    addMarkAction->setCheckable(true);
    connect(addMarkAction, SIGNAL(triggered()),
            engine, SLOT(addMark()));

    addPolyLineAction = new QAction(this);
    addPolyLineAction->setIcon(QIcon(":/images/addpoly.png"));
    addPolyLineAction->setCheckable(true);
    connect(addPolyLineAction, SIGNAL(triggered()),
            engine, SLOT(addPolyLine()));

    addPointAction = new QAction(this);
    addPointAction->setIcon(QIcon(":/images/addpoint.png"));
    addPointAction->setCheckable(true);
    connect(addPointAction, SIGNAL(triggered()),
            engine, SLOT(addPoint()));

    trimSailAction = new QAction(this);
    connect(trimSailAction, SIGNAL(triggered()),
            engine, SLOT(trimSail()));

    autotrimSailAction = new QAction(this);
    connect(autotrimSailAction, SIGNAL(triggered()),
            engine, SLOT(autotrimSail()));

    untrimSailAction = new QAction(this);
    connect(untrimSailAction, SIGNAL(triggered()),
            engine, SLOT(untrimSail()));

    trimJibAction = new QAction(this);
    connect(trimJibAction, SIGNAL(triggered()),
            engine, SLOT(trimJib()));

    autotrimJibAction = new QAction(this);
    connect(autotrimJibAction, SIGNAL(triggered()),
            engine, SLOT(autotrimJib()));

    untrimJibAction = new QAction(this);
    connect(untrimJibAction, SIGNAL(triggered()),
            engine, SLOT(untrimJib()));

    trimSpinAction = new QAction(this);
    connect(trimSpinAction, SIGNAL(triggered()),
            engine, SLOT(trimSpin()));

    autotrimSpinAction = new QAction(this);
    connect(autotrimSpinAction, SIGNAL(triggered()),
            engine, SLOT(autotrimSpin()));

    untrimSpinAction = new QAction(this);
    connect(untrimSpinAction, SIGNAL(triggered()),
            engine, SLOT(untrimSpin()));

    togglePortOverlapAction = new QAction(this);
    togglePortOverlapAction->setCheckable(true);
    connect(togglePortOverlapAction, SIGNAL(triggered()),
            engine, SLOT(togglePortOverlap()));

    toggleStarboardOverlapAction = new QAction(this);
    toggleStarboardOverlapAction->setCheckable(true);
    connect(toggleStarboardOverlapAction, SIGNAL(triggered()),
            engine, SLOT(toggleStarboardOverlap()));

    toggleHiddenAction = new QAction(this);
    toggleHiddenAction->setCheckable(true);
    connect(toggleHiddenAction, SIGNAL(triggered()),
            engine, SLOT(toggleHidden()));

    toggleTextAction = new QAction(this);
    toggleTextAction->setCheckable(true);
    connect(toggleTextAction, SIGNAL(triggered()),
            engine, SLOT(toggleText()));

    toggleSpinAction = new QAction(this);
    toggleSpinAction->setCheckable(true);
    toggleSpinAction->setEnabled(false);
    connect(toggleSpinAction, SIGNAL(triggered()),
            engine, SLOT(toggleSpin()));

    toggleMarkSideAction = new QAction(this);
    connect(toggleMarkSideAction, SIGNAL(triggered()),
            engine, SLOT(toggleMarkSide()));

    toggleMarkArrowAction = new QAction(this);
    toggleMarkArrowAction->setCheckable(true);
    connect(toggleMarkArrowAction, SIGNAL(triggered()),
            engine, SLOT(toggleMarkArrow()));

    toggleMarkZoneAction = new QAction(this);
    toggleMarkZoneAction->setIcon(QIcon(":/images/zone.png"));
    toggleMarkZoneAction->setCheckable(true);
    connect(toggleMarkZoneAction, SIGNAL(triggered()),
            engine, SLOT(toggleMarkZone()));

    setMarkColorAction = new QAction(this);
    setMarkColorAction->setIcon(QIcon());
    connect(setMarkColorAction, SIGNAL(triggered()),
            this, SLOT(setMarkColor()));

    toggleLaylinesAction = new QAction(this);
    toggleLaylinesAction->setIcon(QIcon(":/images/laylines.png"));
    toggleLaylinesAction->setCheckable(true);
    connect(toggleLaylinesAction, SIGNAL(triggered()),
            engine, SLOT(toggleLaylines()));

    toggleMarkLabelAction = new QAction(this);
    toggleMarkLabelAction->setCheckable(true);
    connect(toggleMarkLabelAction, SIGNAL(triggered()),
            engine, SLOT(toggleMarkLabel()));

    editMarkLabelAction = new QAction(this);
    connect(editMarkLabelAction, SIGNAL(triggered()),
            this, SLOT(editMarkLabel()));

    deleteTrackAction = new QAction(this);
    connect(deleteTrackAction, SIGNAL(triggered()),
            engine, SLOT(deleteTrack()));

    deleteAction = new QAction(this);
    connect(deleteAction, SIGNAL(triggered()),
            engine, SLOT(deleteModels()));

    animateAction = new QAction(this);
    animateAction->setIcon(QIcon(":/images/animate.png"));
    animateAction->setCheckable(true);
    connect(animateAction, SIGNAL(triggered(bool)),
            this, SLOT(animate(bool)));

    startAction  = new QAction(this);
    startAction->setIcon(QIcon(":/images/player_play.png"));
    startAction->setEnabled(false);
    connect(startAction, SIGNAL(triggered()),
            engine, SLOT(play()));

    pauseAction  = new QAction(this);
    pauseAction->setIcon(QIcon(":/images/player_pause.png"));
    pauseAction->setEnabled(false);
    pauseAction->setCheckable(true);
    connect(pauseAction, SIGNAL(triggered()),
            engine, SLOT(pause()));

    stopAction = new QAction(this);
    stopAction->setIcon(QIcon(":/images/player_stop.png"));
    stopAction->setEnabled(false);
    connect(stopAction, SIGNAL(triggered()),
            engine, SLOT(stop()));

    loopAction = new QAction(this);
    loopAction->setIcon(QIcon(":/images/player_loop.png"));
    loopAction->setEnabled(false);
    loopAction->setCheckable(true);
    connect(loopAction, SIGNAL(triggered(bool)),
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
    SituationModel *situation = engine->currentModel();

    bool selectedItems = !situation->selectedModels().isEmpty();
    bool selectedBoats = !situation->selectedBoatModels().isEmpty();
    bool selectedMarks = !situation->selectedMarkModels().isEmpty();

    toggleTextAction->setEnabled(selectedItems);

    toggleMarkSideAction->setEnabled(selectedMarks);
    toggleMarkArrowAction->setEnabled(selectedMarks);
    toggleMarkLabelAction->setEnabled(selectedMarks);
    setMarkColorAction->setEnabled(selectedMarks);
    editMarkLabelAction->setEnabled(selectedMarks);
    deleteAction->setEnabled(selectedItems);

    bool allPortSet = 1;
    bool allStarboardSet = 1;
    bool allHiddenSet = 1;
    bool allSpinBoat = 1;
    bool allSpinSet = 1;
    bool allTextSet = 1;
    bool allLaylinesSet = 1;
    bool allMarkArrowSet = 1;
    bool allMarkLabelSet = 1;
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
    foreach(BoatModel *boat, situation->selectedBoatModels()) {
        allPortSet = allPortSet && (boat->overlap() & Boats::port);
        allStarboardSet = allStarboardSet && (boat->overlap() & Boats::starboard);
        allHiddenSet = allHiddenSet && boat->hidden();
        allSpinBoat = allSpinBoat && (boat->hasSpin());
        allSpinSet = allSpinSet && (boat->hasSpin()) && (boat->spin());
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
    toggleSpinAction->setEnabled(selectedBoats && allSpinBoat);

    foreach(PositionModel *position, situation->selectedModels()) {
        allTextSet = allTextSet && (!position->text().isEmpty());
        allLaylinesSet = allLaylinesSet && position->laylines();
    }
    toggleTextAction->setEnabled(situation->selectedModels().size()==1);
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

    foreach(MarkModel *mark, situation->selectedMarkModels()) {
        allMarkArrowSet = allMarkArrowSet && mark->arrowVisible();
        allMarkLabelSet = allMarkLabelSet && mark->labelVisible();
    }
    toggleMarkArrowAction->setChecked(allMarkArrowSet);
    toggleMarkLabelAction->setChecked(allMarkLabelSet);
}

void MainWindow::enterCreateState() {
    SituationView *view = viewList.at(engine->currentIndex());
    view->setCursor(Qt::CrossCursor);
}

void MainWindow::exitCreateState() {
    SituationView *view = viewList.at(engine->currentIndex());
    view->unsetCursor();
}

void MainWindow::cleanState(bool state) {
    SituationModel *situation = engine->currentModel();
    if (situation->fileName().isEmpty())
        saveFileAction->setEnabled(false);
    else
        saveFileAction->setEnabled(!state);
    saveAsAction->setEnabled(!state);
    QString shownName = QFileInfo(situation->fileName()).fileName();
    setWindowTitle(tr("%1 - %2 [*]").arg(tr("Boat Scenario")).arg(shownName));
    if (!state) {
        tabWidget->setTabText(engine->currentIndex(),shownName.append(" *"));
    } else {
        tabWidget->setTabText(engine->currentIndex(),shownName);
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
    trackMenu->addAction(trimJibAction);
    trackMenu->addAction(autotrimJibAction);
    trackMenu->addAction(untrimJibAction);
    trackMenu->addAction(trimSpinAction);
    trackMenu->addAction(autotrimSpinAction);
    trackMenu->addAction(untrimSpinAction);
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
    trackMenu->addSeparator();
    trackMenu->addAction(toggleMarkSideAction);
    trackMenu->addAction(toggleMarkArrowAction);
    trackMenu->addAction(toggleMarkZoneAction);
    trackMenu->addAction(setMarkColorAction);
    trackMenu->addAction(toggleLaylinesAction);
    trackMenu->addAction(toggleMarkLabelAction);
    trackMenu->addAction(editMarkLabelAction);
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
    boatPopup->addAction(trimJibAction);
    boatPopup->addAction(autotrimJibAction);
    boatPopup->addAction(untrimJibAction);
    boatPopup->addAction(trimSpinAction);
    boatPopup->addAction(autotrimSpinAction);
    boatPopup->addAction(untrimSpinAction);
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
    markPopup->addAction(toggleMarkSideAction);
    markPopup->addAction(toggleMarkArrowAction);
    markPopup->addAction(toggleMarkZoneAction);
    markPopup->addAction(toggleLaylinesAction);
    markPopup->addAction(setMarkColorAction);
    markPopup->addAction(toggleMarkLabelAction);
    markPopup->addAction(editMarkLabelAction);
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
    toolbar->addSeparator();

    lookDirectionSlider = new QSlider(Qt::Horizontal);
    lookDirectionSlider->setMaximum(360);
    lookDirectionSlider->setSingleStep(15);
    lookDirectionSlider->setTickInterval(15);
    lookDirectionSlider->setTickPosition(QSlider::TicksBelow);
    toolbar->addWidget(lookDirectionSlider);

    tiltSlider = new QSlider(Qt::Horizontal);
    tiltSlider->setMinimum(0);
    tiltSlider->setMaximum(90);
    tiltSlider->setSingleStep(15);
    tiltSlider->setTickInterval(15);
    tiltSlider->setTickPosition(QSlider::TicksBelow);
    toolbar->addWidget(tiltSlider);

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
    engine->newFile();
    SituationModel *situation = engine->currentModel();
    SituationScene *scene = new SituationScene(situation);
    SituationView *view =new SituationView(scene);

    sceneList.append(scene);
    scene->setDefaultPopup(defaultPopup);
    scene->setBoatPopup(boatPopup);
    scene->setMarkPopup(markPopup);
    scene->setPointPopup(pointPopup);
    viewList.append(view);
    tabWidget->addTab(view, "");

    tabWidget->setCurrentIndex(engine->situationSize() - 1);
    view->setFocus();
    view->setMouseTracking(true);
    if (engine->situationSize() > 1) {
        removeTabAction->setEnabled(true);
        removeTabButton->setEnabled(true);
    }
}

void MainWindow::unsetTab() {
    if (engine->situationSize() == 1) {
        return;
    }

    SituationModel *situation = engine->currentModel();
    SituationScene *scene = sceneList.at(engine->currentIndex());
    StateMachine *machine = situation->stateMachine();
    SituationView *view = viewList.at(engine->currentIndex());

    animate(false);

    disconnect(situation->undoStack(), 0, this, 0);
    disconnect(scene, 0, this, 0);
    disconnect(view, 0, 0, 0);

    disconnect(machine->createState(), 0, this, 0);
    disconnect(machine->createTrackState(), 0, this, 0);
    disconnect(machine->boatSelectionState(), 0, this, 0);
    disconnect(machine->createBoatState(), 0, this, 0);
    disconnect(machine->createMarkState(), 0, this, 0);
    disconnect(machine->createLineState(), 0, this, 0);
    disconnect(machine->pointSelectionState(), 0, this, 0);
    disconnect(machine->createPointState(), 0, this, 0);
    disconnect(machine->animationState(), 0, this, 0);
    disconnect(machine->playState(), 0, this, 0);
    disconnect(machine->pauseState(), 0, this, 0);
    disconnect(machine->stopState(), 0, this, 0);

    disconnect(undoAction, 0, 0, 0);
    disconnect(redoAction, 0, 0, 0);

    disconnect(zoomInAction, 0, 0, 0);
    disconnect(zoomOutAction, 0, 0, 0);
    disconnect(zoomFitAction, 0, 0, 0);
    disconnect(lookDirectionSlider, 0, 0, 0);
    disconnect(lookDirectionSlider);
    disconnect(tiltSlider, 0, 0, 0);
    disconnect(tiltSlider);

    situationWidget->unSetSituation();
}

void MainWindow::setTab(int index) {
    if (index != engine->currentIndex()) {
        unsetTab();
        engine->setIndex(index);
    }
    SituationModel *situation = engine->currentModel();
    SituationScene *scene = sceneList.at(index);
    SituationView *view = viewList.at(index);
    StateMachine *machine = situation->stateMachine();

    connect(machine->createState(), SIGNAL(entered()),
            this, SLOT(enterCreateState()));
    connect(machine->createState(), SIGNAL(exited()),
            this, SLOT(exitCreateState()));
    connect(machine->createTrackState(), SIGNAL(enabledChanged(bool)),
            addTrackAction, SLOT(setEnabled(bool)));
    connect(machine->createTrackState(), SIGNAL(activeChanged(bool)),
            addTrackAction, SLOT(setChecked(bool)));
    addTrackAction->setEnabled(machine->createTrackState()->isEnabled());
    addTrackAction->setChecked(machine->createTrackState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            addBoatAction, SLOT(setEnabled(bool)));
    connect(machine->createBoatState(), SIGNAL(activeChanged(bool)),
            addBoatAction, SLOT(setChecked(bool)));
    addBoatAction->setEnabled(machine->boatSelectionState()->isActive());
    addBoatAction->setChecked(machine->createBoatState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            trimSailAction, SLOT(setEnabled(bool)));
    trimSailAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            autotrimSailAction, SLOT(setEnabled(bool)));
    autotrimSailAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            untrimSailAction, SLOT(setEnabled(bool)));
    untrimSailAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            trimJibAction, SLOT(setEnabled(bool)));
    trimJibAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            autotrimJibAction, SLOT(setEnabled(bool)));
    autotrimJibAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            untrimJibAction, SLOT(setEnabled(bool)));
    untrimJibAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            trimSpinAction, SLOT(setEnabled(bool)));
    trimSpinAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            autotrimSpinAction, SLOT(setEnabled(bool)));
    autotrimSpinAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            untrimSpinAction, SLOT(setEnabled(bool)));
    untrimSpinAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            togglePortOverlapAction, SLOT(setEnabled(bool)));
    togglePortOverlapAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            toggleStarboardOverlapAction, SLOT(setEnabled(bool)));
    toggleStarboardOverlapAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            toggleHiddenAction, SLOT(setEnabled(bool)));
    toggleHiddenAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            flagMenu, SLOT(setEnabled(bool)));
    flagMenu->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            accelerationMenu, SLOT(setEnabled(bool)));
    accelerationMenu->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->boatSelectionState(), SIGNAL(activeChanged(bool)),
            deleteTrackAction, SLOT(setEnabled(bool)));
    deleteTrackAction->setEnabled(machine->boatSelectionState()->isActive());

    connect(machine->createMarkState(), SIGNAL(enabledChanged(bool)),
            addMarkAction, SLOT(setEnabled(bool)));
    connect(machine->createMarkState(), SIGNAL(activeChanged(bool)),
            addMarkAction, SLOT(setChecked(bool)));
    addMarkAction->setEnabled(machine->createMarkState()->isEnabled());
    addMarkAction->setChecked(machine->createMarkState()->isActive());

    connect(machine->createLineState(), SIGNAL(enabledChanged(bool)),
            addPolyLineAction, SLOT(setEnabled(bool)));
    connect(machine->createLineState(), SIGNAL(activeChanged(bool)),
            addPolyLineAction, SLOT(setChecked(bool)));
    addPolyLineAction->setEnabled(machine->createLineState()->isEnabled());
    addPolyLineAction->setChecked(machine->createLineState()->isActive());

    connect(machine->pointSelectionState(), SIGNAL(activeChanged(bool)),
            addPointAction, SLOT(setEnabled(bool)));
    connect(machine->createPointState(), SIGNAL(activeChanged(bool)),
            addPointAction, SLOT(setChecked(bool)));
    addPointAction->setEnabled(machine->pointSelectionState()->isActive());
    addPointAction->setChecked(machine->createPointState()->isActive());

    connect(machine->animationState(), SIGNAL(enabledChanged(bool)),
            animateAction, SLOT(setEnabled(bool)));
    connect(machine->animationState(), SIGNAL(activeChanged(bool)),
            animateAction, SLOT(setChecked(bool)));
    animateAction->setEnabled(machine->animationState()->isEnabled());
    animateAction->setChecked(machine->animationState()->isActive());

    connect(machine->animationState(), SIGNAL(enabledChanged(bool)),
            loopAction, SLOT(setEnabled(bool)));
    connect(machine->animationState(), SIGNAL(activeChanged(bool)),
            loopAction, SLOT(setChecked(bool)));
    loopAction->setEnabled(machine->animationState()->isEnabled());
    loopAction->setChecked(machine->animationState()->isActive());

    connect(machine->playState(), SIGNAL(enabledChanged(bool)),
            startAction, SLOT(setEnabled(bool)));
    connect(machine->playState(), SIGNAL(activeChanged(bool)),
            startAction, SLOT(setChecked(bool)));
    startAction->setEnabled(machine->playState()->isEnabled());
    startAction->setChecked(machine->playState()->isActive());

    connect(machine->pauseState(), SIGNAL(enabledChanged(bool)),
            pauseAction, SLOT(setEnabled(bool)));
    connect(machine->pauseState(), SIGNAL(activeChanged(bool)),
            pauseAction, SLOT(setChecked(bool)));
    pauseAction->setEnabled(machine->pauseState()->isEnabled());
    pauseAction->setChecked(machine->pauseState()->isActive());

    connect(machine->stopState(), SIGNAL(enabledChanged(bool)),
            stopAction, SLOT(setEnabled(bool)));
    connect(machine->stopState(), SIGNAL(activeChanged(bool)),
            stopAction, SLOT(setChecked(bool)));
    stopAction->setEnabled(machine->stopState()->isEnabled());
    stopAction->setChecked(machine->stopState()->isActive());

    connect(undoAction, SIGNAL(triggered()),
            situation->undoStack(), SLOT(undo()));
    connect(situation->undoStack(), SIGNAL(canUndoChanged(bool)),
            undoAction, SLOT(setEnabled(bool)));
    undoAction->setEnabled(situation->undoStack()->canUndo());

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

    connect(view, SIGNAL(lookDirectionChanged(int)),
            lookDirectionSlider, SLOT(setValue(int)));
    connect(view, SIGNAL(tiltChanged(int)),
            tiltSlider, SLOT(setValue(int)));
    lookDirectionSlider->setValue(situation->lookDirection());
    tiltSlider->setValue(situation->tilt());
    connect(lookDirectionSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setLookAt()));
    connect(tiltSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setLookAt()));

    situationWidget->setSituation(situation);

    connect(scene, SIGNAL(selectedModelsChanged()),
            this, SLOT(updateActions()));
    updateActions();
}

void MainWindow::removeTab() {
    int index = tabWidget->currentIndex();
    SituationScene *scene = sceneList.at(index);
    SituationView *view = viewList.at(index);

    if (!maybeSave()) {
        return;
    }

    if (index == engine->situationSize() - 1) {
        tabWidget->setCurrentIndex(index - 1);
    } else if (index != engine->situationSize() - 2) {
        tabWidget->setCurrentIndex(index + 1);
    }
    sceneList.removeAt(index);
    viewList.removeAt(index);

    tabWidget->removeTab(index);
    view->deleteLater();
    scene->deleteLater();
    engine->removeFile(index);
    if (engine->situationSize() == 1) {
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
    settings.setValue("recentList", recentList);
    settings.setValue("fileList",engine->fileList());
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

bool MainWindow::maybeSave() {
    SituationModel *situation = engine->currentModel();
    if (!situation->undoStack()->isClean()) {
        QString shownName = QFileInfo(situation->fileName()).fileName();
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, shownName,
                    tr("The document %1 has been modified.\n"
                        "Do you want to save your changes?").arg(shownName),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return saveSituation("");
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    animate(false);

    while(engine->situationSize()>1) {
        if (!maybeSave()) {
            event->ignore();
            return;
        }
        removeTab();
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

        trimJibAction->setText(tr("Trim Jib"));
        trimJibAction->setShortcut(tr(","));

        autotrimJibAction->setText(tr("Auto Jib"));
        autotrimJibAction->setShortcut(tr("?"));

        untrimJibAction->setText(tr("Untrim Jib"));
        untrimJibAction->setShortcut(tr("."));

        trimSpinAction->setText(tr("Trim Spin"));
        trimSpinAction->setShortcut(tr("Ctrl+,"));

        autotrimSpinAction->setText(tr("Auto Spin"));
        autotrimSpinAction->setShortcut(tr("Ctrl+?"));

        untrimSpinAction->setText(tr("Untrim Spin"));
        untrimSpinAction->setShortcut(tr("Ctrl+."));

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

        toggleMarkSideAction->setText(tr("Toggle &Mark Side"));
        toggleMarkSideAction->setShortcut(tr("Alt+M"));

        toggleMarkArrowAction->setText(tr("Toggle Mark Arro&w"));
        toggleMarkArrowAction->setShortcut(tr("Alt+W"));

        toggleMarkZoneAction->setText(tr("Toggle Mark &Zone"));
        toggleMarkZoneAction->setShortcut(tr("Alt+Z"));

        setMarkColorAction->setText(tr("Set Mark &Color"));
        setMarkColorAction->setShortcut(tr("Alt+C"));

        toggleLaylinesAction->setText(tr("Toggle &Laylines"));
        toggleLaylinesAction->setShortcut(tr("Alt+L"));

        toggleMarkLabelAction->setText(tr("Toggle Mark &Label"));
        toggleMarkLabelAction->setShortcut(tr("Alt+L"));

        editMarkLabelAction->setText(tr("&Edit Mark Label"));
        editMarkLabelAction->setShortcut(tr("Alt+E"));

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
    SituationModel *situation = engine->currentModel();
    SituationView *view = viewList.at(engine->currentIndex());

    if (maybeSave()) {
        // tidy up animation (does no harm if not in animation mode)
        animate(false);
        engine->resetFile();
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
        tabWidget->setCurrentIndex(engine->situationSize() - 1);
    } else {
        // delete situation;
        newFile();
    }

    if (!engine->openFile(fileName)) {
        return;
    }
    situationWidget->update();

    SituationScene *scene = sceneList.at(engine->currentIndex());
    SituationView *view = viewList.at(engine->currentIndex());
    view->centerOn(scene->itemsBoundingRect().center());
    statusbar->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveSituation(QString fileName) {
    SituationModel *situation = engine->currentModel();
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
    if(!engine->saveSituation(name)) {
        return false;
    }
    statusbar->showMessage(tr("File saved"), 2000);
    return true;
}

bool MainWindow::saveFile() {
    SituationModel *situation = engine->currentModel();

    bool animated = situation->stateMachine()->animationState()->isActive();
    animate(false);
    bool saved = saveSituation(situation->fileName());
    if (animated) {
        animate(true);
    }
    return saved;
}

bool MainWindow::saveAs() {
    SituationModel *situation = engine->currentModel();

    bool animated = situation->stateMachine()->animationState()->isActive();
    animate(false);
    bool saved = saveSituation("");
    if (animated) {
        animate(true);
    }
    return saved;
}

void MainWindow::print() {
    SituationModel *situation = engine->currentModel();
    SituationScene *scene = sceneList.at(engine->currentIndex());
    SituationView *view = viewList.at(engine->currentIndex());
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted) {
        return;
    }

    scene->clearSelection();
    SituationPrint printSituation(situation, view);
    printSituation.render(printer.paperRect(QPrinter::Millimeter).adjusted(20, 20, -20, -20));
    printSituation.print(&printer);
}

void MainWindow::printPreview() {
    SituationModel *situation = engine->currentModel();
    SituationScene *scene = sceneList.at(engine->currentIndex());
    SituationView *view = viewList.at(engine->currentIndex());

    scene->clearSelection();
    SituationPrint printSituation(situation, view);
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog dialog(&printer);
    printSituation.render(printer.paperRect(QPrinter::Millimeter).adjusted(20, 20, -20, -20));

    connect(&dialog, SIGNAL(paintRequested(QPrinter*)),
            &printSituation, SLOT(print(QPrinter*)));
    dialog.exec();
}

void MainWindow::exportPdf() {
    SituationModel *situation = engine->currentModel();
    SituationScene *scene = sceneList.at(engine->currentIndex());
    SituationView *view = viewList.at(engine->currentIndex());

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
    scene->clearSelection();
    SituationPrint printSituation(situation, view);
    printSituation.render(printer.paperRect(QPrinter::Millimeter).adjusted(20, 20, -20, -20));
    printSituation.print(&printer);
}

void MainWindow::exportImage() {
    SituationModel *situation = engine->currentModel();
    SituationScene *scene = sceneList.at(engine->currentIndex());
    SituationView *view = viewList.at(engine->currentIndex());

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
    if (!formatsList.contains(QFileInfo(fileName).suffix().toUtf8())) {
#if defined(Q_WS_MAC)
	fileName.append(".png");
#else
	fileName.append(".").append(ext.left(ext.indexOf(' ')).toLower());
#endif
    }

    scene->clearSelection();
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
    SituationModel *situation = engine->currentModel();
    SituationView *view = viewList.at(engine->currentIndex());

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
    if (!newExt.toUtf8().contains("gif")) {
        fileName.append(".gif");
    }

    GifWriter *writer = new GifWriter();

    animate(true, false);
    QProgressDialog progress(tr("Exporting Animation..."), tr("Abort"), 0, situation->animation()->duration(), this);
    progress.setWindowModality(Qt::WindowModal);
    statusbar->showMessage("Exporting animation");
    situation->animation()->setCurrentTime(situation->animation()->duration()/2);
    QPixmap pixmap = view->screenShot();
    QImage shot = pixmap.toImage().convertToFormat(QImage::Format_Indexed8);
    writer->setColorMap(shot);

    QList<QImage*> imageList;
    for (int i=0; i<=situation->animation()->duration(); i+=80) {
        situation->animation()->setCurrentTime(i);
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

void MainWindow::toggleFlag() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        Boats::Flag flag = (Boats::Flag)action->data().toInt();
        engine->toggleFlag(flag);
   }
}

void MainWindow::toggleAcceleration() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        Boats::Acceleration acceleration = (Boats::Acceleration)action->data().toInt();
        engine->toggleAcceleration(acceleration);
    }
}

void MainWindow::setMarkColor() {
    if (! engine->currentModel()->selectedMarkModels().isEmpty()) {
        ColorPickerWidget colorEditor;
        colorEditor.setColor( engine->currentModel()->selectedMarkModels().first()->color());
        engine->setMarkColor(colorEditor.color());
    }
}

void MainWindow::editMarkLabel() {
    SituationModel *situation = engine->currentModel();
    QList<MarkModel *> markList = situation->selectedMarkModels();

    if (! markList.isEmpty()) {
        QString oldText = markList.first()->labelText();
        bool ok;
        QString newText = QInputDialog::getText(this, tr("Edit mark label"), tr("Label text:"), QLineEdit::Normal, oldText, &ok);
        if (ok) {
            engine->editMarkLabel(newText);
        }
    }
}

void MainWindow::setLookAt() {
    engine->setLookAt(lookDirectionSlider->value(), tiltSlider->value());
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
    SituationModel *situation = engine->currentModel();

    if (state) {
        if(!situation->stateMachine()->animationState()->isActive()) {
            engine->animate();

            undoAction->setEnabled(false);
            redoAction->setEnabled(false);
            connect(situation->animation(), SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)),
                    this, SLOT(changeAnimationState(QAbstractAnimation::State, QAbstractAnimation::State)));
            connect(situation->animation(), SIGNAL(timeChanged(int)),
                    animationSlider, SLOT(setValue(int)));
            connect(animationSlider, SIGNAL(valueChanged(int)),
                    situation->animation(), SLOT(setCurrentTime(int)));

            if (interactive) {
                if (!toggleAnimationToolbarAction->isChecked()) {
                    toggleAnimationToolbarAction->setChecked(true);
                }
                animationSlider->setRange(0,situation->animation()->duration());
                animationSlider->setEnabled(true);
                }
            }
    } else {
        if(situation->stateMachine()->animationState()->isActive()) {
            engine->animate();

            undoAction->setEnabled(situation->undoStack()->canUndo());
            redoAction->setEnabled(situation->undoStack()->canRedo());
            disconnect(this, SLOT(changeAnimationState(QAbstractAnimation::State,QAbstractAnimation::State)));
            disconnect(animationSlider, SLOT(setValue(int)));
            disconnect(situation->animation(), SLOT(setCurrentTime(int)));

            animationSlider->setEnabled(false);
            situation->animation()->stop();
        }
    }
}

void MainWindow::loop(bool loop) {
    SituationModel *situation = engine->currentModel();
    if (loop) {
        if (debugLevel & 1 << ANIMATION) std::cout << "loop play" << std::endl;
        situation->animation()->setLoopCount(-1);
    } else {
        if (debugLevel & 1 << ANIMATION) std::cout << "single play" << std::endl;
        situation->animation()->setLoopCount(1);
    }
}

void MainWindow::changeAnimationState(QAbstractAnimation::State newState, QAbstractAnimation::State) {
    switch(newState) {
        case QAbstractAnimation::Running:
            if (debugLevel & 1 << ANIMATION) std::cout << "state running" << std::endl;
            animationSlider->blockSignals(true);
            break;

        case QAbstractAnimation::Paused:
            if (debugLevel & 1 << ANIMATION) std::cout << "state paused" << std::endl;
            animationSlider->blockSignals(false);
            break;

        case QAbstractAnimation::Stopped:
            if (debugLevel & 1 << ANIMATION) std::cout << "state not running" << std::endl;
            animationSlider->blockSignals(false);
            break;
    }
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Boat Scenario"),
	QString("<center><img src=\":/images/about.png\">"
            "<p><b>Boat Scenario</b> - a Race Scenario drawing tool.</p>"
            "<p>Version %1</p></center>"
            "<p>Copyright (C) 2008-2019 Thibaut GRIDEL </p>"
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
