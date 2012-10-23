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
#include "dbusadaptor.h"
#include <QApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QGraphicsView>
#include <QGraphicsObject>
#include <QDBusConnection>

static WindowManager *wmInstance = 0;

WindowManager *WindowManager::instance()
{
    if (!wmInstance)
        wmInstance = new WindowManager(qApp);
    return wmInstance;
}

WindowManager::WindowManager(QObject *parent)
    : QObject(parent), mEngine(0)
{
    createScene();

    new DBusAdaptor(this);
    QDBusConnection::sessionBus().registerService("org.nemomobile.qmlmessages");
    if (!QDBusConnection::sessionBus().registerObject("/", this)) {
        qWarning() << "Cannot register DBus object!";
    }
}

WindowManager::~WindowManager()
{
    delete mWindow.data();
    delete mEngine;
    delete mScene;
}

void WindowManager::createScene()
{
    if (mEngine)
        return;

    mEngine = new QDeclarativeEngine;
    mEngine->rootContext()->setContextProperty("windowManager",
            QVariant::fromValue<QObject*>(this));

    mScene = new QGraphicsScene;
    // From QDeclarativeView
    mScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    mScene->setStickyFocus(true);

    QDeclarativeComponent component(mEngine, QUrl("qrc:qml/main.qml"));
    if (!component.isReady()) {
        qWarning() << component.errors();
    }
    mRootObject = static_cast<QGraphicsObject*>(component.create());
    mScene->addItem(mRootObject);
}

void WindowManager::ensureWindow()
{
    if (mWindow)
        return;

    createScene();

    mWindow = new QGraphicsView;

    QGraphicsView *w = mWindow.data();

    // mWindow is a QWeakPointer, so it'll be cleared on delete
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowTitle(tr("Messages"));
    w->setAttribute(Qt::WA_OpaquePaintEvent);
    w->setAttribute(Qt::WA_NoSystemBackground);
    w->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    w->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    // From QDeclarativeView
    w->setFrameStyle(0);
    w->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    w->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    w->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    w->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    w->viewport()->setFocusPolicy(Qt::NoFocus);
    w->setFocusPolicy(Qt::StrongFocus);

    w->setScene(mScene);

    QDeclarativeItem *i = qobject_cast<QDeclarativeItem*>(mRootObject);
    w->resize(i->width(), i->height());
    w->setSceneRect(QRectF(0, 0, i->width(), i->height()));
}

void WindowManager::showGroupsWindow()
{
    ensureWindow();

    Q_ASSERT(mRootObject);
    bool ok = mRootObject->metaObject()->invokeMethod(mRootObject, "showGroupsList");
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

    Q_ASSERT(mRootObject);
    bool ok = mRootObject->metaObject()->invokeMethod(mRootObject, "showConversation",
            Q_ARG(QVariant, localUid), Q_ARG(QVariant, remoteUid));
    if (!ok)
        qWarning() << Q_FUNC_INFO << "showConversation call failed";

    mWindow.data()->showFullScreen();
    mWindow.data()->activateWindow();
    mWindow.data()->raise();
}

