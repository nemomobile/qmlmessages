import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    color: "white"
    height: targetInput.height + 22

    property alias targetText: targetInput.text

    TextField {
        id: "targetInput"
        placeholderText: "To:"
        focus: true
        anchors.left: parent.left
        anchors.right: contactsBtn.left
        anchors.margins: 10
        y: 10
    }
    
    Button {
        id: contactsBtn
        anchors.top: targetInput.top
        anchors.bottom: targetInput.bottom
        anchors.right: parent.right
        anchors.rightMargin: 10

        text: qsTr("+")

        platformStyle: ButtonStyle {
            buttonWidth: buttonHeight + 10
            buttonHeight: contactsBtn.height
            fontPixelSize: buttonHeight - 10
            background: "image://theme/meegotouch-button-inverted-background"
            textColor: "white"
        }

        onClicked: console.log("Open contacts browsing dialog here")
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 2
        color: "#c0c0c0"
    }
}

