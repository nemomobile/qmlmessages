#ifndef ACCOUNTSMODEL_H
#define ACCOUNTSMODEL_H

#include <QAbstractListModel>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/AccountManager>

class ConversationChannel;

class AccountsModel : public QAbstractListModel
{
    Q_OBJECT

    // Hack necessary for SelectionDialog in AccountSelector.qml.
    // The Qt Components dialog expects the model to have a count
    // property, and won't show any items if it doesn't.
    Q_PROPERTY(int count READ count NOTIFY countChanged);

public:
    enum {
        AccountPtrRole = Qt::UserRole
    };

    AccountsModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE QVariant get(int row, int role = Qt::DisplayRole) const
    {
        return data(index(row, 0), role);
    }

    int count() const { return rowCount(); }

public slots:
    /* For convenience; call ensureTextChat on the account at row to create
     * a text conversation with contactId. Returns a ConversationChannel.
     */
    ConversationChannel *ensureTextChat(int row, const QString &contactId);

signals:
    void countChanged();

private slots:
    void accountManagerReady(Tp::PendingOperation *op);
    void newAccount(const Tp::AccountPtr &account);

private:
    Tp::AccountManagerPtr mAccountManager;
    QList<Tp::AccountPtr> mAccounts;
};

#endif

