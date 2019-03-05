TEMPLATE = app
CONFIG += console

DESTDIR  = $$PWD

INCLUDEPATH += $$PWD/StochasticGroundMotionGenerator/include \
               $$PWD/StochasticGroundMotionGenerator/external/Clara \
               $$PWD/StochasticGroundMotionGenerator/external

SOURCES += $$PWD/StochasticGroundMotionGenerator/src/command_parser.cc \
           $$PWD/StochasticGroundMotionGenerator/src/eq_generator.cc \
           $$PWD/StochasticGroundMotionGenerator/src/main.cc

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
    INCLUDEPATH += $$PWD/StochasticGroundMotionGenerator/external/stochastic_loading/include
    LIBS += $$PWD/StochasticGroundMotionGenerator/external/stochastic_loading/lib/liblstoc.so
}
