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

            delegate: Rectangle {
                color: model.direction == "incoming" ? "steelblue" : "white"
                height: childrenRect.height
                width: parent.width

                Label {
                    text: model.text
                    width: parent.width
                    height: paintedHeight
                }
                // "image://theme/meegotouch-speechbubble-incoming-background" :
            }
        }

        ScrollDecorator {
            flickableItem: messagesView
        }
    }

    TextArea {
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
        }
    }
}

