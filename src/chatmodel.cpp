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

#include "chatmodel.h"
#include "conversationchannel.h"
#include <QDebug>

#include <TelepathyQt4/ReceivedMessage>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Contact>

ChatModel::ChatModel(ConversationChannel *c)
    : QAbstractListModel(c), mConversation(c)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "text";
    roles[ChatDirectionRole] = "direction";
    roles[MessageDateRole] = "date";
    roles[StatusRole] = "status";
    setRoleNames(roles);

    connect(c, SIGNAL(messageSending(QString,Tp::PendingSendMessage*)),
            SLOT(messageSending(QString,Tp::PendingSendMessage*)));
    connect(c, SIGNAL(requestFailed(QString,QString)),
            SLOT(channelRequestFailed(QString,QString)));
}

void ChatModel::setChannel(const Tp::TextChannelPtr &channel)
{
    Q_ASSERT(mChannel.isNull());
    if (!mChannel.isNull())
        return;

    Q_ASSERT(channel->isReady(Tp::TextChannel::FeatureMessageQueue));
    mChannel = channel;

    connect(mChannel.data(), SIGNAL(messageReceived(Tp::ReceivedMessage)),
            SLOT(messageReceived(Tp::ReceivedMessage)));

    QList<Tp::ReceivedMessage> messages = mChannel->messageQueue();
    foreach (Tp::ReceivedMessage msg, messages)
        messageReceived(msg);
}

void ChatModel::messageReceived(const Tp::ReceivedMessage &message)
{
    qDebug() << "ChatModel:" << message.received() << message.text();

    beginInsertRows(QModelIndex(), mMessages.size(), mMessages.size());
    mMessages.append(Message(message.text(), message.messageToken(), message.received(), Incoming));
    endInsertRows();

    mChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);
}

void ChatModel::messageSending(const QString &text, Tp::PendingSendMessage *message)
{
    // XXX should have notification of when the message is actually sent
    QString id = message ? message->sentMessageToken() : QString();

    beginInsertRows(QModelIndex(), mMessages.size(), mMessages.size());
    mMessages.append(Message(text, id, QDateTime::currentDateTime(), Outgoing));
    endInsertRows();
}

void ChatModel::channelRequestFailed(const QString &errorName, const QString &errorMessage)
{
    Q_UNUSED(errorName);

    for (int row = 0; row < mMessages.size(); row++) {
        if (mMessages[row].uniqueId.isNull()) {
            mMessages[row].status = -1;
            emit dataChanged(index(row, 0), index(row, 0));
        }
    }
}

int ChatModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return mMessages.size();
    else
        return 0;
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= mMessages.size())
        return QVariant();

    const Message &message = mMessages[index.row()];
    switch (role) {
        case Qt::DisplayRole: return message.text;
        case ChatDirectionRole: return message.direction;
        case MessageDateRole: return message.date;
        case StatusRole: return message.status;
    }

    return QVariant();
}

int ChatModel::rowForUniqueId(const QString &uniqueId) const
{
    for (int i = mMessages.size()-1; i >= 0; i--) {
        if (mMessages[i].uniqueId == uniqueId)
            return i;
    }
    return -1;
}

ChatModel::Message::Message(const QString &t, const QString &id, const QDateTime &dt, Direction d)
    : text(t), uniqueId(id), date(dt), direction(d)
{
    if (d == Incoming)
        status = 1;
    else
        status = 0;
}

