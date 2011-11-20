/*
 * Copyright 2011 Robin Burchell
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

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

