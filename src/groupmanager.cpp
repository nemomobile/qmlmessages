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

#include "groupmanager.h"
#include "conversationchannel.h"
#include <QThread>
#include <CommHistory/GroupModel>

Q_GLOBAL_STATIC(GroupManager, gmInstance)

GroupManager *GroupManager::instance()
{
    return gmInstance();
}

GroupManager::GroupManager(QObject *parent)
    : QObject(parent)
{
    QThread *modelThread = new QThread(this);
    modelThread->start();

    mGroupModel = new CommHistory::GroupModel(this);
    mGroupModel->setBackgroundThread(modelThread);
    mGroupModel->getGroups();
}

ConversationChannel *GroupManager::getConversationById(int groupid)
{
    QHash<int,ConversationChannel*>::iterator it = groups.find(groupid);
    if (it == groups.end()) {
        ConversationChannel *c = new ConversationChannel(this);
        c->setGroup(groupid);
        addGroup(groupid, c);
        return c;
    }
    return *it;
}

CommHistory::Group GroupManager::groupFromUid(const QString &localUid, const QString &remoteUid)
{
    for (int i = 0, c = mGroupModel->rowCount(); i < c; i++) {
        const CommHistory::Group &g = mGroupModel->group(mGroupModel->index(i, 0));
        if (g.localUid() == localUid && g.remoteUids().size() == 1
                && g.remoteUids().first() == remoteUid) {
            return g;
        }
    }
    return CommHistory::Group();
}

CommHistory::Group GroupManager::groupFromId(int groupid)
{
    for (int i = 0, c = mGroupModel->rowCount(); i < c; i++) {
        const CommHistory::Group &g = mGroupModel->group(mGroupModel->index(i, 0));
        if (g.id() == groupid)
            return g;
    }

    return CommHistory::Group();
}

ConversationChannel *GroupManager::getConversation(const QString &localUid, const QString &remoteUid, bool create)
{
    CommHistory::Group group = groupFromUid(localUid, remoteUid);
    if (group.isValid())
        return getConversationById(group.id());
    if (!create)
        return 0;

    qDebug() << "ConversationChannel creating group for" << localUid << remoteUid;
    group.setLocalUid(localUid);
    group.setRemoteUids(QStringList() << remoteUid);
    group.setChatType(CommHistory::Group::ChatTypeP2P);
    if (!mGroupModel->addGroup(group)) {
        qWarning() << "ConversationChannel failed creating group" << localUid << remoteUid;
        return 0;
    }
    Q_ASSERT(group.isValid());

    ConversationChannel *c = new ConversationChannel(this);
    c->setupGroup(group);
    addGroup(group.id(), c);
    return c;
}

void GroupManager::addGroup(int id, ConversationChannel *c)
{
    Q_ASSERT(!groups.contains(id));
    groups.insert(id, c);
    connect(c, SIGNAL(destroyed(QObject*)), SLOT(groupDestroyed(QObject*)));
}

void GroupManager::groupDestroyed(QObject *obj)
{
    for (QHash<int,ConversationChannel*>::iterator it = groups.begin(); it != groups.end(); it++) {
        if (*it == obj) {
            groups.erase(it);
            break;
        }
    }
}
