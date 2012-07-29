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

/* ConversationPage has two states, depending on if it has an active
 * conversation or not. This is determined by whether the channel property
 * is set. If unset, this is the new conversation page, and some elements
 * are different. */
Page {
    id: conversationPage
    property ConversationChannel channel

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
            text: channel == null ? "" : channel.contactId
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

    // For the new conversation state
    AccountSelector {
        id: accountSelector
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        height: targetEditor.height

        visible: false
    }

    TargetEditBox {
        id: targetEditor
        anchors.top: accountSelector.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        visible: false

        function startConversation() {
            if (targetEditor.text.length < 1 || accountSelector.model == undefined
                || accountSelector.selectedIndex < 0)
                return
            console.log("startConversation");
            channel = accountSelector.model.ensureTextChat(
                    accountSelector.model.selectedIndex, targetEditor.text)
        }
    }

    MessagesView {
        id: messagesView
        anchors {
            top: header.bottom
            bottom: textArea.top
            left: parent.left; right: parent.right
            topMargin: 10; bottomMargin: 10
            leftMargin: 5; rightMargin: 5
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
                enabled: textInput.text.length > 0
                
                platformStyle: ButtonStyle {
                    buttonWidth: 100
                    buttonHeight: textInput.height - 10
                    background: "image://theme/meegotouch-button-inverted-background"
                    disabledBackground: background
                    textColor: "white"
                    disabledTextColor: "lightgray"
                }

                onClicked: {
                    if (conversationPage.state == "new" && targetEditor.text.length > 0) {
                        targetEditor.startConversation()
                    }

                    if (conversationPage.channel !== null && textInput.text.length > 0) {
                        conversationPage.channel.sendMessage(textInput.text)
                        textInput.text = ""
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "active"
            when: conversationPage.channel !== null

            PropertyChanges {
                target: messagesView
                model: channel.model
            }
        },
        State {
            name: "new"
            when: conversationPage.channel == null

            PropertyChanges {
                target: targetEditor
                visible: true
            }

            PropertyChanges {
                target: avatar
                visible: false
            }

            PropertyChanges {
                target: accountSelector
                visible: true
                model: accountsModel
            }

            AnchorChanges {
                target: messagesView
                anchors.top: targetEditor.bottom
            }

            PropertyChanges {
                target: sendBtn
                enabled: textInput.text.length > 0 && targetEditor.text.length > 0
            }
        }
    ]
}

