/*
 * Copyright 2011 Robin Burchell
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import org.nemomobile.qmlmessages 1.0

PageStackWindow {
    id: window 

    initialPage: ConversationListPage {}

    Connections {
        target: clientHandler
        onNewChatModel: {
            pageStack.push(Qt.resolvedUrl("ConversationPage.qml"), { model: chatModel })
        }
    }

    // Shared AccountsModel
    AccountsModel {
        id: accountsModel
    }
}

