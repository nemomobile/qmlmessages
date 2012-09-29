PROJECT_NAME = qmlmessages
QT += dbus declarative svg

CONFIG += link_pkgconfig
PKGCONFIG += commhistory TelepathyQt4 contextprovider-1.0

target.path = $$INSTALL_ROOT/usr/bin
INSTALLS += target

SOURCES += main.cpp \
    accountsmodel.cpp \
    clienthandler.cpp \
    conversationchannel.cpp \
    windowmanager.cpp \
    dbusadaptor.cpp \
    groupmanager.cpp

HEADERS += accountsmodel.h \
    clienthandler.h \
    conversationchannel.h \
    windowmanager.h \
    dbusadaptor.h \
    groupmanager.h

RESOURCES += ../res/res.qrc ../qml/qml.qrc

# do not edit below here
TEMPLATE = app
CONFIG -= app_bundle
TARGET = $$PROJECT_NAME

CONFIG += link_pkgconfig

packagesExist(qdeclarative-boostable) {
    message("Building with qdeclarative-boostable support")
    DEFINES += HAS_BOOSTER
    PKGCONFIG += qdeclarative-boostable
} else {
    warning("qdeclarative-boostable not available; startup times will be slower")
}
