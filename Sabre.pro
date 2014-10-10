#-------------------------------------------------
#
# Project created by QtCreator 2014-08-09T12:47:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sabre
TEMPLATE = app


SOURCES += main.cpp\
    parse.cpp \
    sabre_output.cpp \
    ui_databasedlg.cpp \
    ui_mainwindow.cpp \
    sabre_search.cpp \
    bb_ballpark.cpp \
    bb_defs.cpp \
    bb_game.cpp \
    bb_player.cpp \
    bb_record.cpp \
    bb_stat.cpp \
    bb_team.cpp \
    bb_state.cpp

HEADERS  += \
    parse.h \
    baseball.h \
    sabre_output.h \
    ui_databasedlg.h \
    ui_mainwindow.h \
    sabre_search.h \
    sabre.h \
    bb_ballpark.h \
    bb_defs.h \
    bb_game.h \
    bb_player.h \
    bb_record.h \
    bb_stat.h \
    bb_state.h \
    bb_team.h
