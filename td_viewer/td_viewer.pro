TARGET = td_viewer

TEMPLATE = app

CONFIG += c++11
CONFIG -= qt

DESTDIR = ../bin
OBJECTS_DIR = obj

CONFIG(release, debug|release): DEFINES += NDEBUG

INCLUDEPATH += ../dep/hGLES/include\
               ../dep/\
               ../dep/glm\

SOURCES += \
    td_viewer.cpp\
    ../dep/hGLES/lib_hgles.cpp

HEADERS += \

QMAKE_RPATHDIR += \$\$ORIGIN
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''

LIBS += -L../dep/hGLES/lib -lglfw
# crude hack to determine wheather this is build on a RaspberryPI

exists(/opt/vc/lib/libmmal.so){
DEFINES += USE_PI
QMAKE_CXXFLAGS += -Wno-variadic-macros -std=c++11
}








