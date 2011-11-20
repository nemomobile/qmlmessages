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
    width: parent.width
    height: parent.height

    property string subText: qsTr("You haven't had any chats yet.")
    property string addConversation: qsTr("Start a chat")

    anchors { top: parent.top; right: parent.right; }
    signal clicked()

    Label {
        id: no_conversations
        text: subText
        smooth: true
        anchors {verticalCenter: parent.verticalCenter; topMargin: 40; horizontalCenter: parent.horizontalCenter;}
        opacity: 1
    }

    Button {
        id: button
        text: addConversation
        enabled: true
        anchors{ top: no_conversations.bottom; topMargin: 30; horizontalCenter: no_conversations.horizontalCenter;}
        height: 60
        opacity: 1
        onClicked: {
            root.clicked()
        }
    }
}
