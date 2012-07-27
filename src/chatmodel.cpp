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
    roles[MessageDateRole] = "date";
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
    mMessages.append(Message(message.text(), message.received(), Incoming));
    endInsertRows();

    mChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);
}

void ChatModel::sendMessage(const QString &text)
{
    qDebug() << "ChatModel: sending" << text;

    Q_ASSERT(mChannel->isReady());
    mChannel->send(text);

    // XXX This maybe should use Tp::TextChannel::messageSent to find
    // what was actually sent. Although, this might not be best in the
    // model to begin with.
    beginInsertRows(QModelIndex(), mMessages.size(), mMessages.size());
    mMessages.append(Message(text, QDateTime::currentDateTime(), Outgoing));
    endInsertRows();
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
    }

    return QVariant();
}

ChatModel::Message::Message(const QString &t, const QDateTime &dt, Direction d)
    : text(t), date(dt), direction(d)
{
}

