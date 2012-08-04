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

#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>

#include <TelepathyQt4/TextChannel>

class ConversationChannel;

/* ChatModel contains the history of a conversation, currently associated
 * with exactly one ConversationChannel. It does not handle any Telepathy
 * operations, other than reacting to sent and received messages to update
 * the history. */
class ChatModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Direction)
    
    Q_PROPERTY(ConversationChannel* conversation READ conversation CONSTANT);

public:
    enum {
        ChatDirectionRole = Qt::UserRole,
        MessageDateRole,
        StatusRole
    };

    enum Direction {
        Incoming,
        Outgoing
    };

    ChatModel(ConversationChannel *conversation);

    ConversationChannel *conversation() const { return mConversation; }
    void setChannel(const Tp::TextChannelPtr &channel);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    void messageSending(const QString &text, Tp::PendingSendMessage *message);

private slots:
    void messageReceived(const Tp::ReceivedMessage &message);
    void channelRequestFailed(const QString &errorName, const QString &errorMessage);

private:
    struct Message {
        QString text;
        QString uniqueId;
        QDateTime date;
        Direction direction;
        qint8 status;

        Message(const QString &text, const QString &uniqueId, const QDateTime &date, Direction direction);
    };

    ConversationChannel * const mConversation;
    Tp::TextChannelPtr mChannel;
    QList<Message> mMessages;

    int rowForUniqueId(const QString &uniqueId) const;
};

Q_DECLARE_METATYPE(ChatModel*)

#endif

