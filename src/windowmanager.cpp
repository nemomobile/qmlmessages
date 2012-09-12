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
#include "qmlgroupmodel.h"
#include "dbusadaptor.h"
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDBusConnection>
#ifdef HAS_BOOSTER
#include <applauncherd/MDeclarativeCache>
#endif

Q_GLOBAL_STATIC(WindowManager, wmInstance)

extern QmlGroupModel *groupModel;

WindowManager *WindowManager::instance()
{
    return wmInstance();
}

WindowManager::WindowManager(QObject *parent)
    : QObject(parent)
{
    new DBusAdaptor(this);
    QDBusConnection::sessionBus().registerService("org.nemomobile.qmlmessages");
    if (!QDBusConnection::sessionBus().registerObject("/", this)) {
        qWarning() << "Cannot register DBus object!";
    }
}

WindowManager::~WindowManager()
{
#ifndef HAS_BOOSTER
    delete mWindow;
#endif
}

void WindowManager::showGroupsWindow()
{
    if (!mWindow) {
#ifdef HAS_BOOSTER
        mWindow = MDeclarativeCache::qDeclarativeView();
#else
        mWindow = new QDeclarativeView;
#endif

        QDeclarativeView *w = mWindow.data();

        // mWindow is a QWeakPointer, so it'll be cleared on delete
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setWindowTitle(tr("Messages"));
        w->rootContext()->setContextProperty("clientHandler", QVariant::fromValue<QObject*>(ClientHandler::instance()));
        w->rootContext()->setContextProperty("groupModel", QVariant::fromValue<QObject*>(groupModel));
        w->rootContext()->setContextProperty("groupManager", QVariant::fromValue<QObject*>(GroupManager::instance()));
        w->setSource(QUrl("qrc:qml/qmlmessages/main.qml"));
        w->setAttribute(Qt::WA_OpaquePaintEvent);
        w->setAttribute(Qt::WA_NoSystemBackground);
        w->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
        w->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    }

    // XXX set page for existing windows?

    mWindow.data()->showFullScreen();
    mWindow.data()->activateWindow();
}

