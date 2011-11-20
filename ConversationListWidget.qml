import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    EmptyConversations {
        id: emptyListView
        anchors.fill: parent
        onClicked: {
            pageStack.push(Qt.resolvedUrl("ConversationPage.qml"))
        }
    }

    ListModel {
        id: peopleModel
        ListElement {
            personavatarPath: "image://theme/icon-m-telephony-contact-avatar"
            persondisplayLabel: "Nemo Mobile"
            personfavorite: false
        }
    }

    ListView {
        id: cardListView
        anchors.fill: parent
        snapMode: ListView.SnapToItem
        highlightFollowsCurrentItem: false
        focus: true
        keyNavigationWraps: false
        clip: true
        opacity: 0
        model: peopleModel

        delegate: ConversationListDelegate {
            onClicked: {
                pageStack.push(Qt.resolvedUrl("ConversationPage.qml"))
            }
        }
    }

    Binding {
        target: emptyListView;
        property: "opacity";
        value: ((cardListView.count == 0) ? 1 : 0);
    }
    Binding {
        target: cardListView;
        property: "opacity";
        value: ((cardListView.count > 0) ? 1 : 0);
    }
}
