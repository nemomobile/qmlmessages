#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>

#include <TelepathyQt4/TextChannel>

class ChatModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ChatModel(const Tp::TextChannelPtr &channel, QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private slots:
    void channelReady(Tp::PendingOperation *operation);
    void messageReceived(const Tp::ReceivedMessage &message);

private:
    Tp::TextChannelPtr mChannel;
};

#endif

