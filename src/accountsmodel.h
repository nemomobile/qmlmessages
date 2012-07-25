#ifndef ACCOUNTSMODEL_H
#define ACCOUNTSMODEL_H

#include <QAbstractListModel>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/AccountManager>

class AccountsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    AccountsModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

private slots:
    void accountManagerReady(Tp::PendingOperation *op);
    void newAccount(const Tp::AccountPtr &account);

private:
    Tp::AccountManagerPtr mAccountManager;
};

#endif

