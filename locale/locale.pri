TRANSLATIONS += \
    $$PWD/boats_da.ts \
    $$PWD/boats_de.ts \
    $$PWD/boats_es.ts \
    $$PWD/boats_fr.ts \
    $$PWD/boats_it.ts \
    $$PWD/boats_ja.ts \
    $$PWD/boats_nl.ts \
    $$PWD/boats_pl.ts \
    $$PWD/boats_pt.ts \
    $$PWD/boats_ru.ts \
    $$PWD/boats_sl.ts \
    $$PWD/boats_sv.ts

QM_FILES = $$replace(TRANSLATIONS, "\\.ts", ".qm")

isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}
isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

updatets.commands = $$QMAKE_LUPDATE -locations none -noobsolete $$_PRO_FILE_
QMAKE_EXTRA_TARGETS += updatets

updateqm.depends = updatets
updateqm.input = TRANSLATIONS
updateqm.output = locale/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

rcc.depends = $$QM_FILES
