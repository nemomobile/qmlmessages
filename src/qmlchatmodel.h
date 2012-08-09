#ifndef QMLCHATMODEL_H
#define QMLCHATMODEL_H

#include <CommHistory/ConversationModel>

class QmlChatModel : public CommHistory::ConversationModel
{
    Q_OBJECT
    Q_ENUMS(Direction)

public:
    enum {
        DirectionRole = Qt::UserRole,
        StartTimeRole,
        StatusRole
    };

    enum Direction {
        UnknownDirection = 0,
        Incoming,
        Outgoing
    };

    QmlChatModel(int groupid, QObject *parent = 0);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif

