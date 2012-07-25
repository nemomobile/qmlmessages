#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>

#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/ReceivedMessage>

class ChatModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum {
        ChatDirectionRole = Qt::UserRole
    };

    ChatModel(const Tp::TextChannelPtr &channel, QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private slots:
    void channelReady(Tp::PendingOperation *operation);
    void messageReceived(const Tp::ReceivedMessage &message);

private:
    Tp::TextChannelPtr mChannel;
    QList<Tp::ReceivedMessage> mMessages;
};

Q_DECLARE_METATYPE(ChatModel*)

#endif

