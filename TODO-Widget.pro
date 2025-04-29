#-------------------------------------------------
#
# TODO Widget Application
# 
# Author: Cornebidouil
# Date: Last updated April 28, 2025
#
# Description:
# A Qt-based desktop widget for managing to-do tasks.
# Features include task categories, due dates, priorities,
# system tray integration, and desktop notifications.
#
# Build Requirements:
# - Qt 5.12 or later
# - C++11 or later
#
# Project created by QtCreator 2025-04-15T15:24:55
#
#-------------------------------------------------

# Required Qt modules
QT       += core gui sql svg network

# Add widgets module for Qt 5+
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Application name and template
TARGET = TODO-Widget
TEMPLATE = app

# Compiler warnings for deprecated Qt features
DEFINES += QT_DEPRECATED_WARNINGS

# Uncomment to make compilation fail when using deprecated APIs
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Source files
SOURCES += \
        main.cpp \
    models/task.cpp \
    models/category.cpp \
    models/taskmodel.cpp \
    models/categorymodel.cpp \
    services/databasemanager.cpp \
    services/settingsmanager.cpp \
    services/importexportservice.cpp \
    controllers/taskcontroller.cpp \
    controllers/categorycontroller.cpp \
    controllers/notificationcontroller.cpp \
    views/mainwindow.cpp \
    views/taskitemdelegate.cpp \
    views/taskeditor.cpp \
    views/settingsdialog.cpp \
    views/categoryeditor.cpp \
    views/categorydelegate.cpp \
    views/tasklistview.cpp

# Header files
HEADERS += \
    models/task.h \
    models/category.h \
    models/taskmodel.h \
    models/categorymodel.h \
    services/databasemanager.h \
    services/settingsmanager.h \
    services/importexportservice.h \
    controllers/taskcontroller.h \
    controllers/categorycontroller.h \
    controllers/notificationcontroller.h \
    views/mainwindow.h \
    views/taskitemdelegate.h \
    views/taskeditor.h \
    views/settingsdialog.h \
    views/categoryeditor.h \
    views/categorydelegate.h \
    views/tasklistview.h

# UI form files
FORMS += \
        mainwindow.ui

# Resources (icons, stylesheets, etc.)
RESOURCES += \
    resources/resources.qrc
