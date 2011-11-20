/*
 * Copyright 2011 Robin Burchell
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    PageHeader {
        id: header
        text: qsTr("Messaging")
    }

    ConversationListWidget {
        id: gvp
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "icon-m-common-add"
            onClicked: {
                pageStack.push(Qt.resolvedUrl("ConversationPage.qml"))
            }
        }
    }
}

