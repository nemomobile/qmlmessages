#ifndef CONVERSATIONSMODEL_H
#define CONVERSATIONSMODEL_H

#include <QAbstractListModel>

class ConversationChannel;

class ConversationsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum {
        ConversationRole = Qt::UserRole,
        MessagePreviewRole,
        LastMessageDateRole,
    };

    ConversationsModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    void addChat(ConversationChannel *channel);
    void messagesChanged();
    void conversationChanged();

private:
    QList<ConversationChannel*> mChats;
};

#endif

