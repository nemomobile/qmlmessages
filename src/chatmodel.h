#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>

#include <TelepathyQt4/TextChannel>

/* ChatModel is a dual purpose class at the moment; it acts both as a model for
 * MessagesView, with the text of the conversation, and as the representative
 * class of a conversation. That probably isn't an ideal structure for the long
 * term, and it might be a good idea to split these up. */
class ChatModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Direction)

    Q_PROPERTY(QString contactId READ contactId NOTIFY contactIdChanged)

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
    ChatModel(const Tp::TextChannelPtr &channel, QObject *parent = 0);

    void setChannel(const Tp::TextChannelPtr &channel);

    QString contactId() const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    void sendMessage(const QString &text);

signals:
    void contactIdChanged();

private slots:
    void channelReady(Tp::PendingOperation *operation);
    void messageReceived(const Tp::ReceivedMessage &message);

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

