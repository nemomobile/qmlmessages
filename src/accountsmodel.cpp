#include "accountsmodel.h"
#include "qmlchannelrequest.h"
#include <QDBusConnection>
#include <QDebug>

#include <TelepathyQt4/AccountFactory>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/PendingChannelRequest>

Q_DECLARE_METATYPE(Tp::AccountPtr)

AccountsModel::AccountsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "name";
    roles[AccountPtrRole] = "accountPtr";
    setRoleNames(roles);

    mAccountManager = Tp::AccountManager::create(Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                Tp::Account::FeatureCore));
    connect(mAccountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), SLOT(accountManagerReady(Tp::PendingOperation*)));
    connect(mAccountManager.data(), SIGNAL(newAccount(Tp::AccountPtr)), SLOT(newAccount(Tp::AccountPtr)));
}

void AccountsModel::accountManagerReady(Tp::PendingOperation *op)
{
    foreach (const Tp::AccountPtr &account, mAccountManager->allAccounts()) {
        qDebug() << "Found account" << account->serviceName() << account->uniqueIdentifier() << account->displayName();
        newAccount(account);
    }
}

void AccountsModel::newAccount(const Tp::AccountPtr &account)
{
    // XXX sorting?
    beginInsertRows(QModelIndex(), mAccounts.size(), mAccounts.size());
    mAccounts.append(account);
    endInsertRows();
    emit countChanged();
}

QmlChannelRequest *AccountsModel::ensureTextChat(int row, const QString &contactId) 
{
    Q_ASSERT(row >= 0 && row < mAccounts.size());
    if (row < 0 || row >= mAccounts.size())
        return 0;

    Tp::AccountPtr account = mAccounts[row];
    Q_ASSERT(!account.isNull());
    if (account.isNull())
        return 0;

    qDebug() << "ensureTextChat with" << contactId;
    Tp::PendingChannelRequest *pr = account->ensureTextChat(contactId,
            QDateTime::currentDateTime(),
            QLatin1String("org.freedesktop.Telepathy.Client.qmlmessages"));
    Q_ASSERT(pr);
    if (!pr)
        return 0;

    QmlChannelRequest *re = new QmlChannelRequest;
    connect(re, SIGNAL(finished()), re, SLOT(deleteLater()));
    re->start(pr);
    return re;
}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return mAccounts.size();
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= mAccounts.size())
        return QVariant();

    Tp::AccountPtr account = mAccounts[index.row()];
    Q_ASSERT(!account.isNull());
    if (account.isNull())
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole: return account->displayName();
        case AccountPtrRole: return QVariant::fromValue(account);
    }

    return QVariant();
}

