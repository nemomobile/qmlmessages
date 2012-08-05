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
    roles[StatusMessageRole] = "statusMessage";
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
    if (message.isDeliveryReport()) {
        Tp::ReceivedMessage::DeliveryDetails report = message.deliveryDetails();

        int row = -1;
        if (report.hasOriginalToken())
            row = rowForUniqueId(report.originalToken());
        if (row < 0 && report.hasEchoedMessage()) {
            Tp::Message echo = report.echoedMessage();
            if (!echo.messageToken().isEmpty())
                row = rowForUniqueId(echo.messageToken());

            for (int i = mMessages.size()-1; row < 0 && i >= 0; i--) {
                if (mMessages[i].text == report.echoedMessage().text())
                    row = i;
            }
        }

        if (report.isError())
            qDebug() << "Delivery error:" << row << report.error() << report.debugMessage() << report.dbusError();
        else
            qDebug() << "Delivery success" << row;

        if (row >= 0) {
            mMessages[row].status = report.isError() ? -1 : 1;
            // XXX Figure out decent message text for this case
            mMessages[row].statusDetails = QString("error: %1\ndebug: %2\ndbus: %3")
                .arg(report.error())
                .arg(report.debugMessage())
                .arg(report.dbusError());
            emit dataChanged(index(row, 0), index(row, 0));
        }
    } else {
        beginInsertRows(QModelIndex(), mMessages.size(), mMessages.size());
        mMessages.append(Message(message.text(), message.messageToken(), message.received(), Incoming));
        endInsertRows();

        mChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);
    }
}

void ChatModel::messageSending(const QString &text, Tp::PendingSendMessage *message)
{
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
            mMessages[row].statusDetails = errorMessage;
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
        case StatusMessageRole: return message.statusDetails;
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

