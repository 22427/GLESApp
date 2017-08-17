TARGET = spinning_cube

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
   spinning_cube.cpp\
    ../dep/hGLES/lib_hgles.cpp

HEADERS += \

QMAKE_RPATHDIR += \$\$ORIGIN
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''

# crude hack to determine wheather this is build on a RaspberryPI
exists(/opt/vc/lib/libmmal.so){
DEFINES += USE_PI
LIBS += -L/opt/vc/lib/ -ldl -lbcm_host -lvcos -lvchiq_arm -lGLESv2 -lEGL -lpthread -lrt
INCLUDEPATH += /opt/vc/include/
QMAKE_CXXFLAGS += -Wno-variadic-macros -std=c++11
}else{
LIBS += -L../dep/hGLES/lib -lglfw
}








