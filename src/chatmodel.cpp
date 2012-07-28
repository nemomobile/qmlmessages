#include "chatmodel.h"
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

ChatModel::ChatModel(const Tp::TextChannelPtr &channel, QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "text";
    roles[ChatDirectionRole] = "direction";
    roles[MessageDateRole] = "date";
    setRoleNames(roles);

    setChannel(channel);
}

void ChatModel::setChannel(const Tp::TextChannelPtr &channel)
{
    Q_ASSERT(mChannel.isNull());
    if (!mChannel.isNull())
        return;

    mChannel = channel;

    connect(mChannel.data(), SIGNAL(messageReceived(Tp::ReceivedMessage)),
            SLOT(messageReceived(Tp::ReceivedMessage)));
    connect(mChannel->becomeReady(Tp::TextChannel::FeatureMessageQueue),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(channelReady(Tp::PendingOperation*)));
}

void ChatModel::channelReady(Tp::PendingOperation *op)
{
    Q_UNUSED(op);

    emit contactIdChanged();

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
    if (mChannel.isNull()) {
        qWarning() << "ChatModel: WARN: Cannot sendMessage with no channel";
        return;
    }

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

QString ChatModel::contactId() const
{
    if (mChannel.isNull() || !mChannel->isReady())
        return QString();

    Tp::ContactPtr contact = mChannel->targetContact();
    // XXX Can happen if target is not a single contact type
    if (contact.isNull())
        return tr("Unknown Contact");

    return contact->id();
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

