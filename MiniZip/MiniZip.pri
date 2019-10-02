
INCLUDEPATH += $$PWD

win32{
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
}

HEADERS  += $$PWD/zip.h $$PWD/unzip.h $$PWD/ioapi.h $$PWD/ZipUtils.h

SOURCES += $$PWD/zip.c $$PWD/unzip.c $$PWD/ioapi.c $$PWD/ZipUtils.cpp

linux{
    LIBS += -lz
}

macx{
    LIBS += -lz
}

