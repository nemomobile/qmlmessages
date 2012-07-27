/*
 * Copyright 2011 Intel Corporation.
 * Copyright 2011 Robin Burchell
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: root
    anchors.centerIn: parent
    width: textColumn.width
    height: textColumn.height

    property string subText: qsTr("You haven't had any chats yet.")
    property string addConversation: qsTr("Start a chat")

    signal clicked()

    Column {
        id: textColumn
        width: childrenRect.width
        height: childrenRect.height
        spacing: 15

        Label {
            id: no_conversations
            text: subText
            smooth: true
        }

        Button {
            id: button
            text: addConversation
            height: 60
            onClicked: {
                root.clicked()
            }
        }
    }
}
