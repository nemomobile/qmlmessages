import QtQuick 1.1
import com.nokia.meego 1.0
import org.nemomobile.qmlmessages 1.0

Button {
    id: selectBtn
    
    property alias model: dialog.model

    SelectionDialog {
        id: dialog
        titleText: qsTr("Account")
        selectedIndex: 0
    }

    onClicked: dialog.open()

    states: State {
        name: "active"
        when: dialog.model !== undefined

        PropertyChanges {
            target: selectBtn
            text: dialog.model.get(dialog.selectedIndex)
        }
    }
}

