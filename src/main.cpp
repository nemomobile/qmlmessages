/* Copyright (C) 2012 John Brooks <john.brooks@dereferenced.net>
 * Copyright (C) 2011 Robin Burchell <robin+mer@viroteck.net>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
#include "src/conversationchannel.h"
#include "src/qmlgroupmodel.h"
#include "src/qmlchatmodel.h"

using namespace Tp;

Tp::AccountManagerPtr accountManager;

QmlGroupModel *groupModel = 0;

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

    accountManager = Tp::AccountManager::create(Tp::AccountFactory::create(dbus,
                         Tp::Account::FeatureCore));

    // Set up QML
    qRegisterMetaType<ChatModel*>();
    qmlRegisterType<AccountsModel>("org.nemomobile.qmlmessages", 1, 0, "AccountsModel");
    qmlRegisterUncreatableType<QmlChatModel>("org.nemomobile.qmlmessages", 1, 0, "ChatModel", "Cannot be created");
    qmlRegisterType<ConversationChannel>("org.nemomobile.qmlmessages", 1, 0, "ConversationChannel");

    QmlGroupModel gm;
    groupModel = &gm;

    // Set up view
    view->setWindowTitle(qApp->translate("Window", "Messages"));
    view->rootContext()->setContextProperty("clientHandler", QVariant::fromValue<QObject*>(clientHandler));
    view->rootContext()->setContextProperty("groupModel", QVariant::fromValue<QObject*>(groupModel));
    view->setSource(QUrl("qrc:qml/qmlmessages/main.qml"));
    view->setAttribute(Qt::WA_OpaquePaintEvent);
    view->setAttribute(Qt::WA_NoSystemBackground);
    view->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    view->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    view->showFullScreen();

    return a->exec();
}
