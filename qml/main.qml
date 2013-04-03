/* Copyright (C) 2012 John Brooks <john.brooks@dereferenced.net>
 * Copyright (C) 2011 Robin Burchell <robin+nemo@viroteck.net>
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

import QtQuick 1.1
import com.nokia.meego 1.0
import org.nemomobile.messages.internal 1.0
import org.nemomobile.contacts 1.0
import org.nemomobile.commhistory 1.0

PageStackWindow {
    id: window 

    // Shared AccountsModel
    TelepathyAccountsModel {
        id: accountsModel
    }

    PeopleModel {
        id: peopleModel
    }

    TelepathyChannelManager {
        id: channelManager
    }

    CommGroupManager {
        id: groupManager
    }

    CommGroupModel {
        id: groupModel
        manager: groupManager
    }

    MessagesContextProvider {
        id: contextProvider

        observedInbox: !screen.minimized && pageStack.depth === 1
        observedGroups: {
            if (screen.minimized)
                return [ ]

            var c = [ ]
            try {
                if (pageStack.currentPage && pageStack.currentPage.group)
                    c.push(pageStack.currentPage.group)
            } catch (e) {
            }

            return c
        }
    }

    function showConversation(localUid, remoteUid) {
        if (!groupManager.ready) {
            function delayedShow() {
                if (groupManager.ready) {
                    showConversation(localUid, remoteUid)
                    groupManager.readyChanged.disconnect(delayedShow)
                }
            }
            groupManager.readyChanged.connect(delayedShow)
            return
        }

        var group = channelManager.getConversation(localUid, remoteUid)
        if (!group || group == contextProvider.currentConversation)
            return

        var pages = [ ]
        if (!pageStack.currentPage)
            pages.push(Qt.resolvedUrl("ConversationListPage.qml"))
        pages.push({ page: Qt.resolvedUrl("ConversationPage.qml"), properties: { channel: group } })

        if (pageStack.depth > 1)
            pageStack.replace(pages)
        else
            pageStack.push(pages)
    }

    function showGroupsList() {
        if (!pageStack.currentPage)
            pageStack.push(Qt.resolvedUrl("ConversationListPage.qml"))
        else if (pageStack.depth > 1)
            pageStack.pop(null, true)
    }
}

