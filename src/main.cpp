#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDBusConnection>
#include <QtDeclarative>
#ifdef HAS_BOOSTER
#include <applauncherd/MDeclarativeCache>
#endif

#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/ClientRegistrar>

#include "src/accountsmodel.h"
#include "src/clienthandler.h"
#include "src/chatmodel.h"
#include "src/conversationsmodel.h"
#include "src/qmlchannelrequest.h"

using namespace Tp;

#ifdef HAS_BOOSTER
Q_DECL_EXPORT
#endif
int main(int argc, char **argv)
{
    QApplication *a;
    QDeclarativeView *view;
#ifdef HAS_BOOSTER
    a = MDeclarativeCache::qApplication(argc, argv);
    view = MDeclarativeCache::qDeclarativeView();
#else
    qWarning() << Q_FUNC_INFO << "Warning! Running without booster. This may be a bit slower.";
    QApplication aStack(argc, argv);
    QDeclarativeView viewStack;
    a = &aStack;
    view = &viewStack;
#endif

    // Set up Telepathy
    Tp::registerTypes();
    Tp::enableWarnings(true);

    foreach (QString arg, qApp->arguments()) {
        if (arg == "-debug")
            Tp::enableDebug(true);
    }

    // Features can be requested in the factories here, and are applied to all objects
    // of that type created under the ClientHandler; list them here if they are needed
    // often, or immediately after the instance is created.
    const QDBusConnection &dbus = QDBusConnection::sessionBus();
    ClientRegistrarPtr registrar = ClientRegistrar::create(AccountFactory::create(dbus),
            ConnectionFactory::create(dbus), ChannelFactory::create(dbus),
            ContactFactory::create());
    ClientHandler *clientHandler = new ClientHandler;
    AbstractClientPtr handler = AbstractClientPtr::dynamicCast(SharedPtr<ClientHandler>(clientHandler));
    registrar->registerClient(handler, "qmlmessages");

    // Set up QML
    qRegisterMetaType<ChatModel*>();
    qmlRegisterType<ConversationsModel>("org.nemomobile.qmlmessages", 1, 0, "ConversationsModel");
    qmlRegisterType<AccountsModel>("org.nemomobile.qmlmessages", 1, 0, "AccountsModel");
    qmlRegisterUncreatableType<ChatModel>("org.nemomobile.qmlmessages", 1, 0, "ChatModel", "Cannot be created");
    qmlRegisterType<QmlChannelRequest>("org.nemomobile.qmlmessages", 1, 0, "ChannelRequest");

    // Set up view
    view->rootContext()->setContextProperty("clientHandler", QVariant::fromValue<QObject*>(clientHandler));
    view->setSource(QUrl("qrc:qml/qmlmessages/main.qml"));
    view->setAttribute(Qt::WA_OpaquePaintEvent);
    view->setAttribute(Qt::WA_NoSystemBackground);
    view->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    view->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    view->showFullScreen();

    return a->exec();
}
