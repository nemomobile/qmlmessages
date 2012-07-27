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

Page {
    id: conversationPage
    property variant model

    PageHeader {
        id: header
        color: "#bcbcbc"
        z: 1

        Rectangle {
            anchors.bottom: header.bottom
            anchors.left: header.left
            anchors.right: header.right
            height: 2
            color: "#a0a0a0"
        }

        ToolIcon {
            id: backBtn
            anchors.verticalCenter: parent.verticalCenter
            iconId: "icon-m-toolbar-back"
            onClicked: pageStack.pop()
        }

        Text {
            id: label
            anchors.centerIn: parent
            elide: Text.ElideRight
            smooth: true
            color: "#111111"
            text: model.contactId
            style: Text.Raised
            styleColor: "white"

            font.family: "Droid Sans"
            font.pixelSize: 30
        }

        Image {
            id: avatar
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            source: "image://theme/icon-m-telephony-contact-avatar"
            sourceSize: Qt.size(55, 55)
        }
    }

    Item {
        id: conversationThread
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: textArea.top
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.topMargin: 10
        anchors.bottomMargin: 10

        MessagesView {
            id: messagesView
            anchors.fill: parent
            model: conversationPage.model
        }
    }

    Image {
        id: textArea
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: textInput.height + 22

        source: "image://theme/meegotouch-toolbar-portrait-background"

        TextField {
            id: textInput
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            y: 10
            placeholderText: qsTr("Type a message")

            Button {
                id: sendBtn
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.verticalCenter: textInput.verticalCenter
                anchors.verticalCenterOffset: textInput.hasFocus ? 0 : 1

                text: qsTr("Send")
                
                platformStyle: ButtonStyle {
                    buttonWidth: 100
                    buttonHeight: textInput.height - 10
                    background: "image://theme/meegotouch-button-inverted-background"
                    textColor: "white"
                }

                onClicked: {
                    if (textInput.text.length > 0) {
                        conversationPage.model.sendMessage(textInput.text)
                        textInput.text = ""
                    }
                }
            }
        }
    }
}

