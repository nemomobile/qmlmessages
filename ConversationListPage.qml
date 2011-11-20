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

