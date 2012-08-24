#include "qmlchatmodel.h"
#include <QDebug>

QmlChatModel::QmlChatModel(int groupid, QObject *parent)
    : CommHistory::ConversationModel(parent)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "text";
    roles[DirectionRole] = "direction";
    roles[StatusRole] = "status";
    roles[StartTimeRole] = "date";
    setRoleNames(roles);

    setTreeMode(false);
    getEvents(groupid);
}

QVariant QmlChatModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();
    switch (role) {
        case Qt::DisplayRole: column = FreeText; break;
        case DirectionRole: column = CommHistory::ConversationModel::Direction; break;
        case StartTimeRole: column = StartTime; break;
        case StatusRole: column = Status; break;
    }

    return CommHistory::ConversationModel::data(this->index(index.row(), column, index.parent()), Qt::DisplayRole);
}

