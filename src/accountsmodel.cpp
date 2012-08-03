/* Copyright (C) 2012 John Brooks <john.brooks@dereferenced.net>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "accountsmodel.h"
#include "conversationchannel.h"
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

ConversationChannel *AccountsModel::ensureTextChat(int row, const QString &contactId) 
{
    Q_ASSERT(row >= 0 && row < mAccounts.size());
    if (row < 0 || row >= mAccounts.size())
        return 0;

    Tp::AccountPtr account = mAccounts[row];
    Q_ASSERT(!account.isNull());
    if (account.isNull())
        return 0;

    qDebug() << "ensureTextChat with" << contactId << "on account" << account->displayName();
    Tp::PendingChannelRequest *pr = account->ensureTextChat(contactId,
            QDateTime::currentDateTime(),
            QLatin1String("org.freedesktop.Telepathy.Client.qmlmessages"));
    Q_ASSERT(pr);
    if (!pr)
        return 0;

    ConversationChannel *re = new ConversationChannel;
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

