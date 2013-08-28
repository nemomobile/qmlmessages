PROJECT_NAME = qmlmessages
QT += dbus quick

CONFIG += link_pkgconfig
PKGCONFIG += TelepathyQt5

target.path = $$INSTALL_ROOT/usr/bin
INSTALLS += target

SOURCES += main.cpp \
    windowmanager.cpp \
    dbusadaptor.cpp

HEADERS += windowmanager.h \
    dbusadaptor.h

RESOURCES += ../res/res.qrc ../qml/qml.qrc

# do not edit below here
TEMPLATE = app
CONFIG -= app_bundle
TARGET = $$PROJECT_NAME

CONFIG += link_pkgconfig

packagesExist(qdeclarative5-boostable) {
    message("Building with qdeclarative-boostable support")
    DEFINES += HAS_BOOSTER
    PKGCONFIG += qdeclarative5-boostable
} else {
    warning("qdeclarative-boostable not available; startup times will be slower")
}
