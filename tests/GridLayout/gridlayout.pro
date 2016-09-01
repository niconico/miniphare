TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SUBDIRS += gridlayout


INCLUDEPATH += /opt/local/include \
               ../../src

LIBS += -L/opt/local/lib -lgtest


SOURCES += test_gridlayout.cpp \
           ../../src/grid/gridlayout.cpp \
           ../../src/grid/gridlayoutimplyee.cpp \

HEADERS += ../../src/grid/gridlayout.h \
           ../../src/grid/gridlayoutimplfactory.h \
           ../../src/grid/gridlayoutimplyee.h \
           ../../src/types.h \