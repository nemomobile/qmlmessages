TARGET = messages
PLUGIN_IMPORT_PATH = org/nemomobile/messages
VERSION = $$PROJECT_VERSION

HEADERS += src/messagesmanager.h
SOURCES += src/plugin.cpp \
    src/messagesmanager.cpp

TEMPLATE = lib
CONFIG += qt plugin hide_symbols
QT += declarative dbus

target.path = $$[QT_INSTALL_IMPORTS]/$$PLUGIN_IMPORT_PATH
INSTALLS += target

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$PLUGIN_IMPORT_PATH
INSTALLS += qmldir
