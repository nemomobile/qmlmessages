#ifndef QMLGROUPMODEL_H
#define QMLGROUPMODEL_H

#include <CommHistory/GroupModel>

class QmlGroupModel : public CommHistory::GroupModel
{
public:
    enum {
        LastMessageTextRole = Qt::UserRole,
        LastModifiedRole,
        GroupIdRole
    };

    QmlGroupModel(QObject *parent = 0);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif

