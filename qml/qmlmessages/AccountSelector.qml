import QtQuick 1.1
import com.nokia.meego 1.0
import org.nemomobile.qmlmessages 1.0

Rectangle {
    color: "white"

    property alias model: dialog.model
    property alias selectedIndex: dialog.selectedIndex

    Button {
        id: selectBtn
        anchors.fill: parent
        anchors.margins: 10
        text: "No accounts"
        iconSource: "image://theme/icon-m-toolbar-send-chat"

        onClicked: dialog.open()
    }

    SelectionDialog {
        id: dialog
        titleText: qsTr("Account")
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 2
        color: "#e8e8e8"
    }

    states: State {
        name: "active"
        when: dialog.model !== undefined && dialog.model.count > 0

        PropertyChanges {
            target: selectBtn
            text: dialog.model.get(dialog.selectedIndex)
        }

        PropertyChanges {
            target: dialog
            selectedIndex: 0
        }
    }
}

