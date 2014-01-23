TEMPLATE = app
TARGET = boats
CONFIG += qt \
    warn_on
QT += widgets printsupport

include(animation/animation.pri)
include(graphicsview/graphicsview.pri)
include(itemviews/itemviews.pri)
include(model/model.pri)
include(locale/locale.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    boatapplication.h \
    boats.h \
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
    main.cpp \
    mainwindow.cpp \
    situationprint.cpp \
    situationwidget.cpp \
    trace.cpp \
    undocommands.cpp \
    xmlsituationreader.cpp \
    xmlsituationwriter.cpp

unix_deploy:RESOURCES = boats_unix.qrc

else:RESOURCES = boats.qrc

mac {
    QMAKE_LFLAGS += -static
    QMAKE_INFO_PLIST = Info.plist
    ICON = boats.icns
    mime.path = boats.app/Contents/Resources
    mime.files = xbs.icns
    INSTALLS += mime
}

contains(GIF_EXPORT,1) { 
    DEFINES += GIF_EXPORT
    HEADERS += gifwriter.h
    SOURCES += gifwriter.cpp
    LIBS += -lgif
}

unix_deploy { 
    isEmpty(PREFIX):PREFIX = /usr/local

    target.path = $${PREFIX}/bin
    INSTALLS += target

    desktop.path = $${PREFIX}/share/applications
    desktop.files = boats.desktop
    INSTALLS += desktop

    icons.path = $${PREFIX}/share/icons
    icons.files = images/icons/*
    INSTALLS += icons

    mimetype.path = $${PREFIX}/share/mime
    mimetype.files = boats.xml
    INSTALLS += mimetype

    TRANSLATEDIR = $${PREFIX}/share/boats
    translations.path = $${TRANSLATEDIR}
    translations.files = locale/boats_*.qm
    INSTALLS += translations
}
else:TRANSLATEDIR = ":/locale"

DEFINES += TRANSLATEDIR=\\\"$${TRANSLATEDIR}\\\"

PRE_TARGETDEPS += compiler_updateqm_make_all

MOC_DIR = .moc/

OBJECTS_DIR = .obj/

QMAKE_CFLAGS_RELEASE += -fvisibility=hidden
QMAKE_CXXFLAGS_RELEASE += -fvisibility=hidden -fvisibility-inlines-hidden

win32:RC_FILE = boats.rc
win32:QMAKE_LFLAGS += -static
