#-------------------------------------------------
#
# Project created by QtCreator 2015-08-07T17:08:32
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = DynoDB
CONFIG   += console c++11

QTPLUGIN += qsqlmysql

TEMPLATE = app


SOURCES += main.cpp \
    dynodb.cpp \
    predicate.cpp \
    predicateelement.cpp

HEADERS += \
    dynodb.h \
    predicate.h \
    predicateelement.h \
    datatypes.h
