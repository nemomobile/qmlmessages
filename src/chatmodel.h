#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>

#include <TelepathyQt4/TextChannel>

class ChatModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Direction)

public:
    enum {
        ChatDirectionRole = Qt::UserRole
    };

    enum Direction {
        Incoming,
        Outgoing
    };

    ChatModel(const Tp::TextChannelPtr &channel, QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    void sendMessage(const QString &text);

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

