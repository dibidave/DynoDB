#-------------------------------------------------
#
# Project created by QtCreator 2015-08-07T17:08:32
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = DynoDB
CONFIG   += console

QTPLUGIN += qsqlmysql

TEMPLATE = app


SOURCES += main.cpp \
    dynodb.cpp

HEADERS += \
    dynodb.h
