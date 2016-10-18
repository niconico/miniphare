TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


QMAKE_POST_LINK  = PYTHONPATH=$$PWD:$$PWD/.. $$PWD/faradaytest.py $$OUT_PWD


include(../GTest/GTest.pri)


INCLUDEPATH += ../../src



SOURCES += ../../src/Faraday/faraday.cpp \
           ../../src/Faraday/faradayimpl1d.cpp \
           ../../src/grid/gridlayout.cpp \
           ../../src/grid/gridlayoutimplyee.cpp \
           ../../src/grid/gridlayoutimplinternals.cpp \
           ../../src/Field/field.cpp \
           ../../src/vecfield/vecfield.cpp \
           test_main.cpp \
           test_utilities.cpp \
           test_faraday1d.cpp

HEADERS += ../../src/Faraday/faradayfactory.h \
           ../../src/Faraday/faraday.h \
           ../../src/Faraday/faradayimpl.h \
           ../../src/Faraday/faradayimpl1d.h \
           ../../src/grid/gridlayout.h \
           ../../src/grid/gridlayoutimplfactory.h \
           ../../src/grid/gridlayoutimplyee.h \
           ../../src/grid/gridlayoutimpl.h \
           ../../src/grid/gridlayoutimplinternals.h \
           ../../src/types.h \
           ../../src/constants.h \
           ../../src/utility.h \
           ../../src/Field/field.h \
           ../../src/vecfield/vecfield.h \
           test_faraday.h

OTHER_FILES +=\
    faradaytest.py
