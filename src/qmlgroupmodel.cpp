// XXX header

#include "qmlgroupmodel.h"
#include "qmlchatmodel.h"
#include "conversationchannel.h"

QmlGroupModel::QmlGroupModel(QObject *parent)
    : CommHistory::GroupModel(parent)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "displayName";
    roles[LastMessageTextRole] = "messagePreview";
    roles[LastModifiedRole] = "messageDate";
    roles[ConversationRole] = "conversation";
    setRoleNames(roles);

    getGroups();
}

QVariant QmlGroupModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();

    switch (role) {
        case Qt::DisplayRole: {
            QList<CommHistory::Event::Contact> contacts = group(index).contacts();
            if (!contacts.isEmpty())
                return contacts[0].second;
            column = RemoteUids;
            break;
        }
        case LastMessageTextRole: column = LastMessageText; break;
        case LastModifiedRole: column = LastModified; break;
        case ConversationRole: {
            const CommHistory::Group &g = group(index);
            return QVariant::fromValue<QObject*>(ConversationChannel::channelForGroup(g));
        }
    }

    QVariant re = CommHistory::GroupModel::data(this->index(index.row(), column, index.parent()), Qt::DisplayRole);

    if (column == RemoteUids) {
        re = re.value<QVariantList>()[0];
    }
    return re;
}

