import QtQuick 1.1
import com.nokia.meego 1.0
import org.nemomobile.qmlmessages 1.0

Item {
    property alias model: view.model

    onHeightChanged: view.updateHeight()

    ListView {
        id: view
        spacing: 20
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        cacheBuffer: parent.height

        // Bottom-align chat items when contentHeight is less than the screen.
        // This requires a hack - adjusting the height of the view and anchoring
        // it to the bottom, while disabling interactive scrolling.
        // Also, update contentY to keep the latest item at the bottom.
        onContentHeightChanged: { updateHeight() }
        function updateHeight() {
            height = Math.max(1, Math.min(parent.height, contentHeight))
            interactive = parent.height < contentHeight
            contentY = contentHeight - height
        }

        delegate: BorderImage {
            id: messageBox
            height: messageContent.height + 20
            width: parent.width * 0.8

            Item {
                id: messageContent
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                height: Math.max(messageText.height, 60)
                y: 10

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
                    when: model.direction == ChatModel.Incoming

                    PropertyChanges {
                        target: messageBox
                        x: parent.width - width
                        source: "qrc:/images/incoming.svg"
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
                    when: model.direction == ChatModel.Outgoing

                    PropertyChanges {
                        target: messageBox
                        source: "qrc:/images/outgoing.svg"
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
        flickableItem: view
    }
}

