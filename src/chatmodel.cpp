#include "chatmodel.h"
#include <QDebug>

#include <TelepathyQt4/ReceivedMessage>
#include <TelepathyQt4/PendingReady>

ChatModel::ChatModel(const Tp::TextChannelPtr &channel, QObject *parent)
    : QAbstractListModel(parent), mChannel(channel)
{
    connect(mChannel.data(), SIGNAL(messageReceived(Tp::ReceivedMessage)),
            SLOT(messageReceived(Tp::ReceivedMessage)));
    connect(mChannel->becomeReady(Tp::TextChannel::FeatureMessageQueue),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(channelReady(Tp::PendingOperation*)));
}

void ChatModel::channelReady(Tp::PendingOperation *op)
{
    Q_UNUSED(op);

    QList<Tp::ReceivedMessage> messages = mChannel->messageQueue();
    foreach (Tp::ReceivedMessage msg, messages)
        messageReceived(msg);
}

void ChatModel::messageReceived(const Tp::ReceivedMessage &message)
{
    qDebug() << "ChatModel:" << message.received() << message.text();
    mChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);
}

int ChatModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

