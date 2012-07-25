#include "accountsmodel.h"
#include <TelepathyQt4/AccountFactory>
#include <TelepathyQt4/PendingReady>
#include <QDBusConnection>
#include <QDebug>

AccountsModel::AccountsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    mAccountManager = Tp::AccountManager::create(Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                Tp::Account::FeatureCore));
    connect(mAccountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), SLOT(accountManagerReady(Tp::PendingOperation*)));
    connect(mAccountManager.data(), SIGNAL(newAccount(Tp::AccountPtr)), SLOT(newAccount(Tp::AccountPtr)));
}

void AccountsModel::accountManagerReady(Tp::PendingOperation *op)
{
    foreach (const Tp::AccountPtr &account, mAccountManager->allAccounts()) {
        qDebug() << "Found account" << account->serviceName() << account->uniqueIdentifier() << account->displayName();
    }
}

void AccountsModel::newAccount(const Tp::AccountPtr &account)
{

}

int AccountsModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant AccountsModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

