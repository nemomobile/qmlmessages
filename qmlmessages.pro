PROJECT_NAME = qmlmessages
QT += dbus declarative svg

INCLUDEPATH += /usr/include/telepathy-1.0/
LIBS += -ltelepathy-qt4
CXXFLAGS += -fPIC

CONFIG += link_pkgconfig
PKGCONFIG += commhistory

target.path = $$INSTALL_ROOT/usr/bin
INSTALLS += target

desktop.files = $${PROJECT_NAME}.desktop
desktop.path = $$INSTALL_ROOT/usr/share/applications
INSTALLS += desktop

SOURCES += src/main.cpp \
    src/accountsmodel.cpp \
    src/clienthandler.cpp \
    src/chatmodel.cpp \
    src/conversationsmodel.cpp \
    src/conversationchannel.cpp

HEADERS += src/accountsmodel.h \
    src/clienthandler.h \
    src/chatmodel.h \
    src/conversationsmodel.h \
    src/conversationchannel.h

RESOURCES += res/res.qrc qml/qml.qrc

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
