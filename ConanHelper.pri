nning Conan to install dependencies
message(Running Conan)
CONAN_INSTALL_COMMAND=cd $$OUT_PWD && conan install $$PWD -s build_type=$$BUILD_TYPE --build missing
message($$CONAN_INSTALL_COMMAND)
system($$CONAN_INSTALL_COMMAND)

#Set output directories for Windows
win32{
    DESTDIR=$$OUT_PWD/$$BUILD_TYPE
    OBJECTS_DIR=$$OUT_PWD/$$BUILD_TYPE/.moc
    MOC_DIR=$$OUT_PWD/$$BUILD_TYPE/.moc
    UI_DIR=$$OUT_PWD/$$BUILD_TYPE/.ui
    RCC_DIR=$$OUT_PWD/$$BUILD_TYPE/.rcc
}

#Conan setup to include generated pri
CONFIG += conan_basic_setup
include($$OUT_PWD/conanbuildinfo.pri)