#-------------------------------------------------
#
# Project created by QtCreator 2013-12-25T18:09:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += serialport
} else {
    include(../qt-qtserialport/src/serialport/qt4support/serialport.prf)
}

TARGET = greenHouse
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
