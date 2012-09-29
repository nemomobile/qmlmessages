/* Copyright (C) 2012 John Brooks <john.brooks@dereferenced.net>
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

#include "windowmanager.h"
#include "clienthandler.h"
#include "groupmanager.h"
#include "dbusadaptor.h"
#include "conversationchannel.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QDBusConnection>
#include <ContextProvider>
#ifdef HAS_BOOSTER
#include <applauncherd/MDeclarativeCache>
#endif

#include <CommHistory/GroupModel>

Q_GLOBAL_STATIC(WindowManager, wmInstance)

WindowManager *WindowManager::instance()
{
    return wmInstance();
}

WindowManager::WindowManager(QObject *parent)
    : QObject(parent), mCurrentGroup(0)
{
    new DBusAdaptor(this);
    QDBusConnection::sessionBus().registerService("org.nemomobile.qmlmessages");
    if (!QDBusConnection::sessionBus().registerObject("/", this)) {
        qWarning() << "Cannot register DBus object!";
    }

    ContextProvider::Service *cpService = new ContextProvider::Service(QDBusConnection::SessionBus,
            "org.nemomobile.qmlmessages.context", this);
    propObservedConversation = new ContextProvider::Property(*cpService, "Messaging.ObservedConversation",
            this);
    if (propObservedConversation && propObservedConversation->isSet())
        propObservedConversation->unsetValue();
}

WindowManager::~WindowManager()
{
#ifndef HAS_BOOSTER
    delete mWindow;
#endif
}

void WindowManager::ensureWindow()
{
    if (!mWindow) {
#ifdef HAS_BOOSTER
        mWindow = MDeclarativeCache::qDeclarativeView();
#else
        mWindow = new QDeclarativeView;
#endif

        QDeclarativeView *w = mWindow.data();
        mCurrentGroup = 0;

        // mWindow is a QWeakPointer, so it'll be cleared on delete
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setWindowTitle(tr("Messages"));
        w->rootContext()->setContextProperty("windowManager",
                QVariant::fromValue<QObject*>(this));
        w->rootContext()->setContextProperty("groupManager",
                QVariant::fromValue<QObject*>(GroupManager::instance()));
        w->rootContext()->setContextProperty("groupModel",
                QVariant::fromValue<QObject*>(GroupManager::instance()->groupModel()));
        w->setSource(QUrl("qrc:qml/main.qml"));
        w->setAttribute(Qt::WA_OpaquePaintEvent);
        w->setAttribute(Qt::WA_NoSystemBackground);
        w->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
        w->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    }
}

void WindowManager::showGroupsWindow()
{
    ensureWindow();

    QGraphicsObject *root = mWindow.data()->rootObject();
    if (!root)
        qFatal("No root object in window");
    bool ok = root->metaObject()->invokeMethod(root, "showGroupsList");
    if (!ok)
        qWarning() << Q_FUNC_INFO << "showGroupsList call failed";

    mWindow.data()->showFullScreen();
    mWindow.data()->activateWindow();
    mWindow.data()->raise();
}

void WindowManager::showConversation(const QString &localUid, const QString &remoteUid, unsigned type)
{
    Q_UNUSED(type);
    ensureWindow();

    qDebug() << Q_FUNC_INFO << localUid << remoteUid << type;
    ConversationChannel *group = GroupManager::instance()->getConversation(localUid, remoteUid);
    if (!group) {
        qWarning() << Q_FUNC_INFO << "could not create group";
        return;
    }

    QGraphicsObject *root = mWindow.data()->rootObject();
    if (!root)
        qFatal("No root object in window");
    bool ok = root->metaObject()->invokeMethod(root, "showConversation", Q_ARG(QVariant, QVariant::fromValue<QObject*>(group)));
    if (!ok)
        qWarning() << Q_FUNC_INFO << "showConversation call failed";

    mWindow.data()->showFullScreen();
    mWindow.data()->activateWindow();
    mWindow.data()->raise();
}

void WindowManager::updateCurrentGroup(ConversationChannel *g)
{
    if (g == mCurrentGroup)
        return;

    mCurrentGroup = g;
    emit currentGroupChanged(g);

    if (g) {
        QVariantList observed;
        observed << g->localUid() << g->contactId() << CommHistory::Group::ChatTypeP2P;
        propObservedConversation->setValue(observed);
    } else
        propObservedConversation->unsetValue();
}

