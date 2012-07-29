#include "conversationsmodel.h"
#include "conversationchannel.h"
#include "chatmodel.h"

ConversationsModel::ConversationsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "displayName";
    roles[ConversationRole] = "conversation";
    roles[MessagePreviewRole] = "messagePreview";
    roles[LastMessageDateRole] = "messageDate";
    setRoleNames(roles);
}

void ConversationsModel::addChat(ConversationChannel *conversation)
{
    if (mChats.contains(conversation))
        return;

    Q_ASSERT(conversation->model());
    connect(conversation->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(messagesChanged()));
    connect(conversation, SIGNAL(contactIdChanged()), SLOT(conversationChanged()));

    beginInsertRows(QModelIndex(), 0, 0);
    mChats.insert(0, conversation);
    endInsertRows();
}

void ConversationsModel::messagesChanged()
{
    ChatModel *model = qobject_cast<ChatModel*>(sender());
    Q_ASSERT(model);
    if (!model)
        return;

    int row;
    for (row = 0; row < mChats.size(); ++row) {
        if (mChats[row]->model() == model)
            break;
    }
    Q_ASSERT(row < mChats.size());
    if (row >= mChats.size())
        return;

    emit dataChanged(index(row, 0), index(row, 0));
}

void ConversationsModel::conversationChanged()
{
    ConversationChannel *channel = qobject_cast<ConversationChannel*>(sender());
    Q_ASSERT(channel);
    if (!channel)
        return;

    int row = mChats.indexOf(channel);
    Q_ASSERT(row >= 0);
    if (row < 0)
        return;

    emit dataChanged(index(row, 0), index(row, 0));
}

int ConversationsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return mChats.size();
}

QVariant ConversationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= mChats.size())
        return QVariant();

    ConversationChannel *c = mChats[index.row()];

    switch (role) {
        case Qt::DisplayRole: return c->contactId();
        case ConversationRole: return QVariant::fromValue<QObject*>(c);
        case MessagePreviewRole: {
            ChatModel *model = c->model();
            QModelIndex index = model->index(model->rowCount()-1, 0);
            if (index.isValid())
                return model->data(index, Qt::DisplayRole);
            else
                return QString();
        }
        case LastMessageDateRole: {
            ChatModel *model = c->model();
            QModelIndex index = model->index(model->rowCount()-1, 0);
            if (index.isValid())
                return model->data(index, ChatModel::MessageDateRole);
            else
                return QString();
        }
    }

    return QVariant();
}

