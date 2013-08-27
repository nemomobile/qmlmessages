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

#include <QGuiApplication>
#include <QDBusConnection>
#ifdef HAS_BOOSTER
#include <MDeclarativeCache>
#endif

#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <TelepathyQt/Constants>
#include <TelepathyQt/Debug>
#include <TelepathyQt/Types>

#include "windowmanager.h"

#ifdef HAS_BOOSTER
Q_DECL_EXPORT
#endif
int main(int argc, char **argv)
{
    QQuickView *view;
#ifdef HAS_BOOSTER 
    QGuiApplication *application;
    application = MDeclarativeCache::qApplication(argc, argv);
    view = MDeclarativeCache::qQuickView();
#else
    QGuiApplication *application;
    qWarning() << Q_FUNC_INFO << "Warning! Running without booster. This may be a bit slower.";
    QGuiApplication stackApp(argc, argv);
    QQuickView stackView;
    application = &stackApp;
    view = &stackView;
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
        else if (arg == "-prestart")
            showWindow = false;
    }

    QObject::connect(view->engine(), SIGNAL(quit()), application, SLOT(quit()));
    view->setSource(QUrl("qrc:/qml/main.qml"));

    QObject *object = (QObject *)view->rootObject();
    WindowManager *wm = WindowManager::instance( object, view );
    view->rootContext()->setContextProperty("wManager", wm);    
    if (showWindow)
    {
        wm->showGroupsWindow();
    }

    return application->exec();
}
