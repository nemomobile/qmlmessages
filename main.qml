import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: window 
    showToolBar: true;

    initialPage: ConversationListPage {}

    ToolBar {
        anchors.bottom: parent.bottom
    }
}

