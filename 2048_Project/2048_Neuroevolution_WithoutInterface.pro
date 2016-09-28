QT += core
QT -= gui

TARGET = 2048_Neuroevolution_WithoutInterface
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += main.cpp \
    game.cpp \
    chromosome.cpp \
    genalgorithm.cpp \
    geneticinterface.cpp \
    individual.cpp \
    matrix.cpp \
    modnet.cpp \
    neuron.cpp \
    util.cpp

HEADERS += \
    game.h \
    chromosome.h \
    genalgorithm.h \
    geneticinterface.h \
    individual.h \
    matrix.h \
    modnet.h \
    neuron.h \
    util.h

DISTFILES +=

