#include "messagesmanager.h"
#include <QDBusInterface>
#include <QDBusPendingCall>

MessagesManager::MessagesManager(QObject *parent)
    : QObject(parent)
{
    interface = new QDBusInterface("org.nemomobile.qmlmessages", "/", "org.nemomobile.qmlmessages",
            QDBusConnection::sessionBus(), this);
}

void MessagesManager::showGroupsWindow()
{
    interface->asyncCall("showGroupsWindow");
}

void MessagesManager::startConversation(const QString &localAccount, const QString &remoteUid)
{
    // 0 is ChatTypeP2P; this API doesn't support multiple contacts currently.
    interface->asyncCall("startConversation", localAccount, remoteUid, (unsigned)0);
}

void MessagesManager::startSMS(const QString &phoneNumber)
{
    interface->asyncCall("startConversation", "ring/tel/account0", phoneNumber, (unsigned)0);
}

