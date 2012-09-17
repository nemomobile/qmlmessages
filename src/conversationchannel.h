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
#include <TelepathyQt4/ReceivedMessage>
#include <CommHistory/Group>
#include <CommHistory/ConversationModel>

/* ConversationChannel handles the relationship between a commhistory
 * group and the associated Telepathy channel.
 *
 * setGroup associates the instance with a commhistory group, either by
 * groupId or the combination of localUid and remoteUid. When using
 * UIDs, if the group does not exist, it will be created immediately.
 *
 * A text channel and ConversationModel will be automatically established.
 */
class ConversationChannel : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
   
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString contactId READ contactId NOTIFY contactIdChanged)
    Q_PROPERTY(QString localUid READ localUid CONSTANT)

    Q_PROPERTY(QObject* model READ model NOTIFY chatModelReady)

    friend class GroupManager;

public:
    enum State {
        Null,
        PendingRequest,
        Requested,
        PendingReady,
        Ready,
        Error
    };

    virtual ~ConversationChannel();

    State state() const { return mState; }
    QString contactId() const { return mContactId; }
    QString localUid() const { return mLocalUid; }
    QObject *model() const { return mModel; }

    Q_INVOKABLE void ensureChannel();
    void setChannel(const Tp::ChannelPtr &channel);

public slots:
    void sendMessage(const QString &text);

signals:
    void stateChanged(int newState);
    void chatModelReady(QObject *model);
    void contactIdChanged();

    void requestSucceeded();
    void requestFailed(const QString &errorName, const QString &errorMessage);

private slots:
    void channelRequestCreated(const Tp::ChannelRequestPtr &request);
    void channelRequestSucceeded(const Tp::ChannelPtr &channel);
    void channelRequestFailed(const QString &errorName, const QString &errorMessage);
    void channelReady();

    void messageReceived(const Tp::ReceivedMessage &message);

    void channelInvalidated(Tp::DBusProxy *proxy, const QString &errorName, const QString &errorMessage);

private:
    Tp::PendingChannelRequest *mPendingRequest;
    Tp::ChannelRequestPtr mRequest;
    Tp::ChannelPtr mChannel;
    State mState;
    CommHistory::ConversationModel *mModel;

    int mGroupId;
    QString mContactId;
    QString mLocalUid;

    QList<QString> mPendingMessages;

    ConversationChannel(QObject *parent = 0);

    void setState(State newState);
    void start(Tp::PendingChannelRequest *request);

    /* Set commhistory group by ID. Group must exist in the GroupModel. */
    void setGroup(int groupid);
    void setupGroup(const CommHistory::Group &group);
};

#endif

