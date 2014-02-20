#-------------------------------------------------
#
# Project created by QtCreator 2014-02-16T00:30:06
#
#-------------------------------------------------

QT += core \
      network \
      testlib

QT       -= gui

TARGET = nodexmpp
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += qxt
QXT     += core
TEMPLATE = app


SOURCES += main.cpp \
    xmpp_server.cpp \
    service.cpp \
    xmpp_client.cpp

HEADERS += \
    xmpp_server.h \
    main.h \
    service.h \
    xmpp_client.h


LIBS += -lQxtCore \
        -lqxmpp


INCLUDEPATH += ./externals/libqxt/
INCLUDEPATH += ./externals/libqxt/src/core
INCLUDEPATH += ./externals/qxmpp/src/base
INCLUDEPATH += ./externals/qxmpp/src/client
INCLUDEPATH += ./externals/qxmpp/src/server
