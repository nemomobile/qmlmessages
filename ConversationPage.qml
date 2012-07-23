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
        text: qsTr("Nemo Mobile")
    }

    Item {
        id: conversationThread
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: textArea.bottom

        ListModel {
            id: messagesModel

            ListElement {
                direction: "incoming"
                text: "hey"
            }

            ListElement {
                direction: "outgoing"
                text: "hello"
            }
            ListElement {
                direction: "outgoing"
                text: "world"
            }
            ListElement {
                direction: "incoming"
                text: "this isn't a program, silly"
            }
        }

        ListView {
            id: messagesView
            anchors.fill: parent
            clip: true
            model: messagesModel
            spacing: 20

            delegate: BorderImage {
                id: messageBox
                height: 100
                width: parent.width * 0.8

                Item {
                    id: messageContent
                    anchors.fill: parent
                    anchors.margins: 10

                    Label {
                        id: messageText
                        text: model.text
                        width: parent.width
                    }
                }

                // This should use meegotouch's speechbubble theme elements, but those SVG group
                // images are not supported in qt-components currently. incoming.svg and outgoing.svg
                // are extracted from the group SVG in meegotouch's base theme and included here.
                states: [
                    State {
                        name: "incoming"
                        when: model.direction == "incoming"

                        PropertyChanges {
                            target: messageBox
                            x: parent.width - width
                            source: "incoming.svg"
                            border.left: 24
                            border.right: 24
                            border.top: 24
                            border.bottom: 24
                        }

                        PropertyChanges {
                            target: messageContent
                            anchors.rightMargin: 20
                        }
                    },
                    State {
                        name: "outgoing"
                        when: model.direction == "outgoing"

                        PropertyChanges {
                            target: messageBox
                            source: "outgoing.svg"
                            border.left: 24
                            border.right: 24
                            border.top: 24
                            border.bottom: 24
                        }

                        PropertyChanges {
                            target: messageContent
                            anchors.leftMargin: 20
                        }
                    }
                ]
            }
        }

        ScrollDecorator {
            flickableItem: messagesView
        }
    }

    TextField {
        id: textArea
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        placeholderText: qsTr("Enter a message to send.")
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "icon-m-toolbar-back"
            onClicked: {
                pageStack.pop()
            }
        }

        ToolIcon {
            iconId: "icon-m-toolbar-send-sms"
            onClicked: {
                console.log("Sending message!")
                pageStack.pop()
            }
        }
    }
}

