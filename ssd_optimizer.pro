#-------------------------------------------------
#
# Project created by QtCreator 2016-01-16T20:16:51
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ssd_optimizer
TEMPLATE = app


SOURCES += main.cpp\
        ssd_optimizer.cpp \
    windows_service.cpp \
    windows_registry.cpp \
    windows_console.cpp \
    windows_wmi.cpp \
    windows_drive_identifiaction.cpp

HEADERS  += ssd_optimizer.h \
    windows_service.h \
    windows_registry.h \
    windows_console.h \
    windows_wmi.h \
    windows_drive_identifiaction.h

FORMS    += ssd_optimizer.ui

win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings

win32 {
    CONFIG += embed_manifest_exe
    QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'
}
