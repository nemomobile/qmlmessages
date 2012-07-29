#include "chatmodel.h"
#include "conversationchannel.h"
#include <QDebug>

#include <TelepathyQt4/ReceivedMessage>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Contact>

ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "text";
    roles[ChatDirectionRole] = "direction";
    roles[MessageDateRole] = "date";
    setRoleNames(roles);
}

void ChatModel::setChannel(const Tp::TextChannelPtr &channel, ConversationChannel *c)
{
    Q_ASSERT(mChannel.isNull());
    if (!mChannel.isNull())
        return;

    Q_ASSERT(channel->isReady(Tp::TextChannel::FeatureMessageQueue));
    mChannel = channel;

    connect(mChannel.data(), SIGNAL(messageReceived(Tp::ReceivedMessage)),
            SLOT(messageReceived(Tp::ReceivedMessage)));
    connect(c, SIGNAL(messageSending(QString)), SLOT(messageSent(QString)));

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

void ChatModel::messageSent(const QString &text)
{
    // XXX should have notification of when the message is actually sent
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

