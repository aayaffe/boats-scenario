TEMPLATE = app
CONFIG += qt warn_on

qml {
    TARGET = boats-qml
    QT += quick qml widgets

    HEADERS += \
        boats.h \
        boatsengine.h \
        commontypes.h \
        trace.h \
        undocommands.h \
        xmlsituationreader.h \
        xmlsituationwriter.h

    SOURCES += \
        boats.cpp \
        boatsengine.cpp \
        main_qml.cpp \
        trace.cpp \
        undocommands.cpp \
        xmlsituationreader.cpp \
        xmlsituationwriter.cpp

    qml_files.files = boats.qml
    INSTALL += qml_files
}
else {
    TARGET = boats
    QT += widgets printsupport

    include(animation/animation.pri)
    include(graphicsview/graphicsview.pri)
    include(itemviews/itemviews.pri)
    include(locale/locale.pri)

    HEADERS += \
        boatapplication.h \
        boats.h \
        boatsengine.h \
        commontypes.h \
        mainwindow.h \
        situationprint.h \
        situationwidget.h \
        trace.h \
        undocommands.h \
        xmlsituationreader.h \
        xmlsituationwriter.h

    SOURCES += \
        boatapplication.cpp \
        boats.cpp \
        boatsengine.cpp \
        main.cpp \
        mainwindow.cpp \
        situationprint.cpp \
        situationwidget.cpp \
        trace.cpp \
        undocommands.cpp \
        xmlsituationreader.cpp \
        xmlsituationwriter.cpp

    contains(GIF_EXPORT,1) {
        DEFINES += GIF_EXPORT
        HEADERS += gifwriter.h
        SOURCES += gifwriter.cpp
        LIBS += -lgif
    }

    PRE_TARGETDEPS += compiler_updateqm_make_all
}

include(model/model.pri)
INCLUDEPATH += $$PWD

mac {
    QMAKE_LFLAGS += -static
    QMAKE_INFO_PLIST = resources/Info.plist
    ICON = resources/boats.icns
    mime.path = boats.app/Contents/Resources
    mime.files = resources/xbs.icns
    INSTALLS += mime
}

win32 {
    RC_FILE = resources/boats.rc
    QMAKE_LFLAGS += -static
}

unix_deploy { 
    isEmpty(PREFIX):PREFIX = /usr/local

    RESOURCES = boats_unix.qrc

    target.path = $${PREFIX}/bin
    INSTALLS += target

    desktop.path = $${PREFIX}/share/applications
    desktop.files = resources/boats.desktop
    INSTALLS += desktop

    icons.path = $${PREFIX}/share/icons
    icons.files = images/icons/*
    INSTALLS += icons

    mimetype.path = $${PREFIX}/share/mime
    mimetype.files = resources/boats.xml
    INSTALLS += mimetype

    TRANSLATEDIR = $${PREFIX}/share/boats
    translations.path = $${TRANSLATEDIR}
    translations.files = locale/boats_*.qm
    INSTALLS += translations
}
else {
    RESOURCES = boats.qrc
    TRANSLATEDIR = ":/locale"
}

DEFINES += TRANSLATEDIR=\\\"$${TRANSLATEDIR}\\\"

MOC_DIR = .moc/
OBJECTS_DIR = .obj/

QMAKE_CFLAGS_RELEASE += -fvisibility=hidden
QMAKE_CXXFLAGS_RELEASE += -fvisibility=hidden -fvisibility-inlines-hidden
