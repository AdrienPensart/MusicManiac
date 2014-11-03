#-------------------------------------------------
#
# Project created by QtCreator 2014-10-27T20:13:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = musicman
TEMPLATE = app
LIBS += -ltag

SOURCES += main.cpp\
    MusicFile.cpp \
    MusicFolderModel.cpp \
    MainWindow.cpp \
    MP3File.cpp \
    FLACFile.cpp \
    MusicFileFactory.cpp \
    MusicDebugger.cpp

HEADERS  += \
    MusicFile.hpp \
    MusicFolderModel.hpp \
    MP3File.hpp \
    MainWindow.hpp \
    FLACFile.hpp \
    MusicFileFactory.hpp \
    MusicDebugger.hpp \
    NonCopyable.hpp \
    Singleton.hpp

FORMS    += mainwindow.ui
