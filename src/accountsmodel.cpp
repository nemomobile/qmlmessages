#include "accountsmodel.h"
#include <TelepathyQt4/AccountFactory>
#include <TelepathyQt4/PendingReady>
#include <QDBusConnection>
#include <QDebug>

AccountsModel::AccountsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "modelData";
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
    }

    return QVariant();
}

