QT += dbus

INCLUDEPATH += /usr/include/telepathy-1.0/
LIBS += -ltelepathy-qt4

SOURCES += src/main.cpp \
    src/accountsmodel.cpp \
    src/clienthandler.cpp \
    src/chatmodel.cpp

HEADERS += src/accountsmodel.h \
    src/clienthandler.h \
    src/chatmodel.h

RESOURCES += res.qrc qml/qml.qrc
