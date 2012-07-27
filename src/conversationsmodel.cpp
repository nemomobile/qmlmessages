#include "conversationsmodel.h"
#include "chatmodel.h"

ConversationsModel::ConversationsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "displayName";
    roles[ChatModelRole] = "chatModel";
    roles[MessagePreviewRole] = "messagePreview";
    roles[LastMessageDateRole] = "messageDate";
    setRoleNames(roles);
}

void ConversationsModel::addChat(ChatModel *model)
{
    if (mChats.contains(model))
        return;

    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(messagesChanged()));

    beginInsertRows(QModelIndex(), 0, 0);
    mChats.insert(0, model);
    endInsertRows();
}

void ConversationsModel::messagesChanged()
{
    ChatModel *model = qobject_cast<ChatModel*>(sender());
    Q_ASSERT(model);
    if (!model)
        return;

    int row = mChats.indexOf(model);
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

    ChatModel *model = mChats[index.row()];

    switch (role) {
        case Qt::DisplayRole: return model->contactId();
        case ChatModelRole: return QVariant::fromValue<QObject*>(model);
        case MessagePreviewRole: {
            QModelIndex index = model->index(model->rowCount()-1, 0);
            if (index.isValid())
                return model->data(index, Qt::DisplayRole);
            else
                return QString();
        }
        case LastMessageDateRole: {
            QModelIndex index = model->index(model->rowCount()-1, 0);
            if (index.isValid())
                return model->data(index, ChatModel::MessageDateRole);
            else
                return QString();
        }
    }

    return QVariant();
}

