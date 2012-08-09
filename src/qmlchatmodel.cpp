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
    qDebug("getEvents");
    getEvents(groupid);
    qDebug("done with getEvents");
}

QVariant QmlChatModel::data(const QModelIndex &index, int role) const
{
    qDebug() << Q_FUNC_INFO << index << role;
    int column = index.column();
    switch (role) {
        case Qt::DisplayRole: column = FreeText; break;
        case DirectionRole: column = CommHistory::ConversationModel::Direction; break;
        case StartTimeRole: column = StartTime; break;
        case StatusRole: column = Status; break;
    }

    return CommHistory::ConversationModel::data(this->index(index.row(), column, index.parent()), Qt::DisplayRole);
}

