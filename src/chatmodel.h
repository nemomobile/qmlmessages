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

public:
    enum {
        ChatDirectionRole = Qt::UserRole,
        MessageDateRole
    };

    enum Direction {
        Incoming,
        Outgoing
    };

    ChatModel(QObject *parent = 0);

    void setChannel(const Tp::TextChannelPtr &channel, ConversationChannel *c);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    void messageReceived(const Tp::ReceivedMessage &message);
    void messageSent(const QString &text);

private:
    struct Message {
        QString text;
        QDateTime date;
        Direction direction;

        Message(const QString &text, const QDateTime &date, Direction direction);
    };

    Tp::TextChannelPtr mChannel;
    QList<Message> mMessages;
};

Q_DECLARE_METATYPE(ChatModel*)

#endif

