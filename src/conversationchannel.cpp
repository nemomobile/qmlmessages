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

#include "conversationchannel.h"
#include "clienthandler.h"
#include "qmlchatmodel.h"

#include <TelepathyQt4/ChannelRequest>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Contact>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>

QHash<int,ConversationChannel*> ConversationChannel::groupIdMap;

// XXX
extern Tp::AccountManagerPtr accountManager;

ConversationChannel *ConversationChannel::channelForGroup(const CommHistory::Group &group)
{
    Q_ASSERT(group.isValid());
    ConversationChannel *re = groupIdMap.value(group.id());
    if (!re) {
        re = new ConversationChannel;
        re->setCommGroup(group);
        Q_ASSERT(groupIdMap.value(group.id()) == re);
    }
    return re;
}

ConversationChannel::ConversationChannel(QObject *parent)
    : QObject(parent), mPendingRequest(0), mState(Null), mModel(0)
{
}

ConversationChannel::~ConversationChannel()
{
    if (mGroup.isValid()) {
        Q_ASSERT(groupIdMap.value(mGroup.id()) == this);
        groupIdMap.remove(mGroup.id());
    }
    Q_ASSERT(!groupIdMap.values().contains(this));
}

void ConversationChannel::setCommGroup(const CommHistory::Group &group)
{
    Q_ASSERT(!mGroup.isValid());
    mGroup = group;
    Q_ASSERT(!groupIdMap.contains(group.id()));
    groupIdMap.insert(group.id(), this);

    mModel = new QmlChatModel(group.id(), this);
    emit chatModelReady(mModel);

    qDebug() << Q_FUNC_INFO << mGroup.localUid() << mGroup.remoteUids().value(0);

    // XXX wait for account manager if necessary?
    Tp::AccountPtr account = accountManager->accountForPath(mGroup.localUid());
    Q_ASSERT(account);
    Q_ASSERT(account->isReady());
    Tp::PendingChannelRequest *req = account->ensureTextChat(mGroup.remoteUids().value(0),
            QDateTime::currentDateTime(),
            QLatin1String("org.freedesktop.Telepathy.Client.qmlmessages"));
    start(req, mGroup.remoteUids().value(0));
}

void ConversationChannel::start(Tp::PendingChannelRequest *pendingRequest,
        const QString &contactId)
{
    Q_ASSERT(state() == Null || state() == Error);
    Q_ASSERT(!mPendingRequest);
    if (state() != Null && state() != Error)
        return;

    mPendingRequest = pendingRequest;
    connect(mPendingRequest, SIGNAL(channelRequestCreated(Tp::ChannelRequestPtr)),
            SLOT(channelRequestCreated(Tp::ChannelRequestPtr)));

    mContactId = contactId;
    emit contactIdChanged();

    setState(PendingRequest);
}

void ConversationChannel::setChannel(const Tp::ChannelPtr &c)
{
    Q_ASSERT(mChannel.isNull());
    Q_ASSERT(!c.isNull());
    if (!mChannel.isNull() || c.isNull())
        return;

    mChannel = c;
    connect(mChannel->becomeReady(Tp::TextChannel::FeatureMessageQueue),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(channelReady()));

    setState(PendingReady);
}

void ConversationChannel::channelRequestCreated(const Tp::ChannelRequestPtr &r)
{
    Q_ASSERT(state() == PendingRequest);
    Q_ASSERT(mRequest.isNull());
    Q_ASSERT(!r.isNull());
    if (state() != PendingRequest)
        return;

    qDebug() << Q_FUNC_INFO;

    mRequest = r;
    connect(mRequest.data(), SIGNAL(succeeded(Tp::ChannelPtr)),
            SLOT(channelRequestSucceeded(Tp::ChannelPtr)));
    connect(mRequest.data(), SIGNAL(failed(QString,QString)),
            SLOT(channelRequestFailed(QString,QString)));

    mPendingRequest = 0;
    setState(Requested);

    ClientHandler::instance()->addChannelRequest(mRequest, this);
}

void ConversationChannel::channelRequestSucceeded(const Tp::ChannelPtr &channel)
{
    Q_ASSERT(state() == Requested);
    Q_ASSERT(!mRequest.isNull());
    // Telepathy docs note that channel may be null if the dispatcher is too old.
    Q_ASSERT(!channel.isNull());
    if (channel.isNull()) {
        qWarning() << Q_FUNC_INFO << "channel is null (dispatcher too old?)";
        return;
    }

    qDebug() << Q_FUNC_INFO;
    setChannel(channel);
    mRequest.reset();
    emit requestSucceeded();
}

void ConversationChannel::channelRequestFailed(const QString &errorName,
        const QString &errorMessage)
{
    Q_ASSERT(state() == Requested);

    mRequest.reset();
    setState(Error);
    emit requestFailed(errorName, errorMessage);

    qDebug() << Q_FUNC_INFO << errorName << errorMessage;
}

void ConversationChannel::channelReady()
{
    Q_ASSERT(state() == PendingReady);
    Q_ASSERT(!mChannel.isNull());
    if (state() != PendingReady || mChannel.isNull())
        return;

    Tp::TextChannelPtr textChannel = Tp::SharedPtr<Tp::TextChannel>::dynamicCast(mChannel);
    Q_ASSERT(!textChannel.isNull());

    Tp::ContactPtr contact = mChannel->targetContact();
    if (contact.isNull())
        mContactId = tr("Unknown Contact");
    else
        mContactId = contact->id();
    emit contactIdChanged();

    setState(Ready);

    if (!mPendingMessages.isEmpty())
        qDebug() << Q_FUNC_INFO << "Sending buffered messages";
    foreach (QString msg, mPendingMessages)
        textChannel->send(msg);
    mPendingMessages.clear();
}

void ConversationChannel::setState(State newState)
{
    if (mState == newState)
        return;

    mState = newState;
    emit stateChanged(newState);
}

void ConversationChannel::sendMessage(const QString &text)
{
    if (mChannel.isNull() || !mChannel->isReady()) {
        Q_ASSERT(state() != Ready);
        qDebug() << Q_FUNC_INFO << "Buffering:" << text;
        mPendingMessages.append(text);
        emit messageSending(text, NULL);
        return;
    }

    Tp::TextChannelPtr textChannel = Tp::SharedPtr<Tp::TextChannel>::dynamicCast(mChannel);
    Q_ASSERT(!textChannel.isNull());
    if (textChannel.isNull()) {
        qWarning() << Q_FUNC_INFO << "Channel is not a text channel; cannot send messages";
        return;
    }

    qDebug() << Q_FUNC_INFO << text;
    Tp::PendingSendMessage *msg = textChannel->send(text);
    emit messageSending(text, msg);
}

