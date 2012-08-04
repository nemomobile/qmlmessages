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

#ifndef CONVERSATIONCHANNEL_H
#define CONVERSATIONCHANNEL_H

#include <QObject>
#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/ChannelRequest>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/PendingSendMessage>

class ChatModel;

/* ConversationChannel represents a Tp::Channel, from pending requests
 * through to the lifetime of the channel itself, and provides operations
 * on that channel.
 *
 * They can be created either by starting a new channel, e.g. with
 * Tp::Account::ensureTextChat or AccountsModel::ensureTextChat, and calling
 * start(), or by ClientHandler in response to an incoming channel (which goes
 * through setChannel()).
 *
 * Currently, notification when they're created is done entirely through
 * ClientHandler, but that should probably change. It's also currently
 * responsible for creating the ChatModel, but that might be better done within
 * QML now.
 *
 * XXX The lifetime and deletion of this object is currently undefined, and it's
 * expected to leak.
 * XXX It might be nice to use PIMPL, too.
 */
class ConversationChannel : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
   
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString contactId READ contactId NOTIFY contactIdChanged)

    Q_PROPERTY(ChatModel* model READ model NOTIFY chatModelReady)

public:
    enum State {
        Null,
        PendingRequest,
        Requested,
        PendingReady,
        Ready,
        Error
    };

    ConversationChannel(QObject *parent = 0);

    Q_INVOKABLE void start(Tp::PendingChannelRequest *request, const QString &contactId);
    void setChannel(const Tp::ChannelPtr &channel);

    State state() const { return mState; }
    QString contactId() const { return mContactId; }
    ChatModel *model() const { return mModel; }

public slots:
    void sendMessage(const QString &text);

signals:
    void stateChanged(int newState);
    void chatModelReady(ChatModel *model);
    void contactIdChanged();

    void requestSucceeded();
    void requestFailed(const QString &errorName, const QString &errorMessage);

    void messageSending(const QString &text, Tp::PendingSendMessage *message);

private slots:
    void channelRequestCreated(const Tp::ChannelRequestPtr &request);
    void channelRequestSucceeded(const Tp::ChannelPtr &channel);
    void channelRequestFailed(const QString &errorName, const QString &errorMessage);
    void channelReady();

private:
    Tp::PendingChannelRequest *mPendingRequest;
    Tp::ChannelRequestPtr mRequest;
    Tp::ChannelPtr mChannel;
    State mState;
    ChatModel *mModel;

    QString mContactId;

    QList<QString> mPendingMessages;

    void setState(State newState);
};

#endif

