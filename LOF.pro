#-------------------------------------------------
#
# Project created by QtCreator 2017-10-09T18:47:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = LOF
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    lof.cpp

HEADERS  += mainwindow.h \
    qcustomplot/qcustomplot.h \
    lof.h

FORMS    += mainwindow.ui
