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

#include <CommHistory/GroupModel>

#include "src/windowmanager.h"
#include "src/accountsmodel.h"
#include "src/clienthandler.h"
#include "src/conversationchannel.h"

using namespace Tp;

Tp::AccountManagerPtr accountManager;

CommHistory::GroupModel *groupModel = 0;

#ifdef HAS_BOOSTER
Q_DECL_EXPORT
#endif
int main(int argc, char **argv)
{
#ifdef HAS_BOOSTER
    MDeclarativeCache::qApplication(argc, argv);
#else
    qWarning() << Q_FUNC_INFO << "Warning! Running without booster. This may be a bit slower.";
    QApplication a(argc, argv);
#endif

    // Set up Telepathy
    Tp::registerTypes();
    Tp::enableWarnings(true);

    bool showWindow = true;
    foreach (QString arg, qApp->arguments()) {
        if (arg == "-debug")
            Tp::enableDebug(true);
        else if (arg == "-background")
            showWindow = false;
    }

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
    qmlRegisterType<AccountsModel>("org.nemomobile.qmlmessages", 1, 0, "AccountsModel");
    qmlRegisterUncreatableType<ConversationChannel>("org.nemomobile.qmlmessages", 1, 0, "ConversationChannel", "");

    CommHistory::GroupModel gm;
    gm.getGroups();
    groupModel = &gm;

    WindowManager *wm = WindowManager::instance();
    if (showWindow)
        wm->showGroupsWindow();
    else 
        // Stay open persistently if not started with UI
        qApp->setQuitOnLastWindowClosed(false);

    return qApp->exec();
}
