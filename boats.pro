TEMPLATE = app
CONFIG += qt warn_on

HEADERS += \
        boats.h \
        boatsengine.h \
        commontypes.h \
        enablestate.h \
        statemachine.h \
        trace.h \
        undocommands.h \
        xmlsituationreader.h \
        xmlsituationwriter.h

SOURCES += \
        boats.cpp \
        boatsengine.cpp \
        enablestate.cpp \
        statemachine.cpp \
        trace.cpp \
        undocommands.cpp \
        xmlsituationreader.cpp \
        xmlsituationwriter.cpp

qml {
    TARGET = boats-qml
    QT += quick qml widgets
    DEFINES += QML

    SOURCES += \
        main_qml.cpp

    qml_files.files = boats.qml
    INSTALL += qml_files
}

else {
    TARGET = boats
    QT += widgets printsupport

    include(graphicsview/graphicsview.pri)
    include(itemviews/itemviews.pri)
    include(locale/locale.pri)

    HEADERS += \
        boatapplication.h \
        mainwindow.h \
        situationprint.h \
        situationwidget.h

    SOURCES += \
        boatapplication.cpp \
        main.cpp \
        mainwindow.cpp \
        situationprint.cpp \
        situationwidget.cpp

    contains(GIF_EXPORT,1) {
        DEFINES += GIF_EXPORT
        HEADERS += gifwriter.h
        SOURCES += gifwriter.cpp
        LIBS += -lgif
    }

    PRE_TARGETDEPS += compiler_updateqm_make_all

    QMAKE_CFLAGS_RELEASE += -fvisibility=hidden
    QMAKE_CXXFLAGS_RELEASE += -fvisibility=hidden -fvisibility-inlines-hidden

}

include(animation/animation.pri)
include(model/model.pri)
INCLUDEPATH += $$PWD

RESOURCES += images.qrc

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

    target.path = $${PREFIX}/bin
    INSTALLS += target

    desktop.path = $${PREFIX}/share/applications
    desktop.files = resources/boats.desktop
    INSTALLS += desktop

    icons.path = $${PREFIX}/share/icons
    icons.files = images/icons/*
    INSTALLS += icons

    mimetype.path = $${PREFIX}/share/mime/packages
    mimetype.files = resources/boats.xml
    INSTALLS += mimetype

    TRANSLATEDIR = $${PREFIX}/share/boats
    translations.path = $${TRANSLATEDIR}
    translations.files = locale/boats_*.qm
    INSTALLS += translations
}
else {
    !qml {
        RESOURCES += locales.qrc
        TRANSLATEDIR = ":/locale"
    }
}

DEFINES += TRANSLATEDIR=\\\"$${TRANSLATEDIR}\\\"

MOC_DIR = .moc/
OBJECTS_DIR = .obj/
