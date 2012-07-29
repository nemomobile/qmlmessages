#include "conversationchannel.h"
#include "clienthandler.h"
#include "chatmodel.h"

#include <TelepathyQt4/ChannelRequest>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Contact>

ConversationChannel::ConversationChannel(QObject *parent)
    : QObject(parent), mPendingRequest(0), mState(Null), mModel(0)
{
}

void ConversationChannel::start(Tp::PendingChannelRequest *pendingRequest)
{
    Q_ASSERT(state() == Null || state() == Error);
    Q_ASSERT(!mPendingRequest);
    if (state() != Null && state() != Error)
        return;

    mPendingRequest = pendingRequest;
    connect(mPendingRequest, SIGNAL(channelRequestCreated(Tp::ChannelRequestPtr)),
            SLOT(channelRequestCreated(Tp::ChannelRequestPtr)));

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

    if (!mModel) {
        // XXX This is ugly repetition, necessary for incoming chats
        // which don't hit channelRequestCreated (but that can't be
        // moved either, due to ClientHandler::addChannelRequest)
        mModel = new ChatModel(this);
        emit chatModelReady(mModel);
    }

    setState(PendingReady);
}

void ConversationChannel::channelRequestCreated(const Tp::ChannelRequestPtr &r)
{
    Q_ASSERT(state() == PendingRequest);
    Q_ASSERT(mRequest.isNull());
    Q_ASSERT(!r.isNull());
    if (state() != PendingRequest)
        return;

    mRequest = r;
    connect(mRequest.data(), SIGNAL(succeeded(Tp::ChannelPtr)),
            SLOT(channelRequestSucceeded(Tp::ChannelPtr)));
    connect(mRequest.data(), SIGNAL(failed(QString,QString)),
            SLOT(channelRequestFailed(QString,QString)));

    mPendingRequest = 0;
    setState(Requested);

    // XXX is this the best place to create? And object lifetime may be wrong.
    mModel = new ChatModel(this);
    foreach (QString msg, mPendingMessages)
        mModel->messageSent(msg);
    emit chatModelReady(mModel);

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
}

void ConversationChannel::channelReady()
{
    Q_ASSERT(state() == PendingReady);
    Q_ASSERT(!mChannel.isNull());
    if (state() != PendingReady || mChannel.isNull())
        return;

    Q_ASSERT(mModel);
    Tp::TextChannelPtr textChannel = Tp::SharedPtr<Tp::TextChannel>::dynamicCast(mChannel);
    Q_ASSERT(!textChannel.isNull());
    if (!textChannel.isNull())
        mModel->setChannel(textChannel, this);

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

QString ConversationChannel::contactId() const
{
    if (mChannel.isNull() || !mChannel->isReady())
        return QString();

    Tp::ContactPtr contact = mChannel->targetContact();
    // XXX Can happen if target is not a single contact type
    if (contact.isNull())
        return tr("Unknown Contact");

    return contact->id();
}

void ConversationChannel::sendMessage(const QString &text)
{
    if (mChannel.isNull() || !mChannel->isReady()) {
        Q_ASSERT(state() != Ready);
        qDebug() << Q_FUNC_INFO << "Buffering:" << text;
        mPendingMessages.append(text);
        emit messageSending(text);
        return;
    }

    Tp::TextChannelPtr textChannel = Tp::SharedPtr<Tp::TextChannel>::dynamicCast(mChannel);
    Q_ASSERT(!textChannel.isNull());
    if (textChannel.isNull()) {
        qWarning() << Q_FUNC_INFO << "Channel is not a text channel; cannot send messages";
        return;
    }

    qDebug() << Q_FUNC_INFO << text;
    textChannel->send(text);
    emit messageSending(text);
}

