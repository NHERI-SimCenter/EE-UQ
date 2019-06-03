TEMPLATE = app
CONFIG += console c++11

DESTDIR  = $$PWD
SOURCES += SiteResponse.cpp

macx{
    INCLUDEPATH+="/usr/local/jansson/include"
    LIBS+="/usr/local/jansson/lib/libjansson.a"
}

#Assuming jansson library share the same parent folder with the app and is built with the same compiler
win32{
    INCLUDEPATH+="../../../jansson/build/include"
    LIBS+="../../../jansson/build/x64/Release/jansson.lib"
}

unix:!macx{
    LIBS+="/usr/lib/x86_64-linux-gnu/libjansson.a"
}
