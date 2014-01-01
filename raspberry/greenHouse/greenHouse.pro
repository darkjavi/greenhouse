#-------------------------------------------------
#
# Project created by QtCreator 2013-12-25T18:09:29
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += serialport
} else {
    include(qserialport/serialport-lib.pri)
}

TARGET = greenHouse
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vpparserserial.cpp \
    vpparser.cpp \
    vp2dsimplegraph.cpp \
    ghcontroller.cpp \
    ghserialcontroller.cpp \
    ghtcpserver.cpp

HEADERS  += mainwindow.h \
    vpparserserial.h \
    vpparser.h \
    vp2dsimplegraph.h \
    sleeper.h \
    ghcontroller.h \
    ghserialcontroller.h \
    ghtcpserver.h

FORMS    += mainwindow.ui \
    vp2dsimplegraph.ui
