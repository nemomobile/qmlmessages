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

    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "text";
    roles[ChatDirectionRole] = "direction";
    setRoleNames(roles);
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

    beginInsertRows(QModelIndex(), mMessages.size(), mMessages.size());
    mMessages.append(message);
    endInsertRows();

    mChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);
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

    const Tp::ReceivedMessage &message = mMessages[index.row()];
    switch (role) {
        case Qt::DisplayRole: return message.text();
        case ChatDirectionRole: return "incoming";
    }

    return QVariant();
}

