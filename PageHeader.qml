/*
 * Copyright 2011 Robin Burchell
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    id: detailHeader
    height: 72;
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    color: "#FF6600"
    property alias text: label.text
    property alias content: othercontent.children

    Item {
        id: othercontent
        width: childrenRect.width
        height: childrenRect.height
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    Label {
        id: label
        anchors.left: othercontent.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        elide: Text.ElideRight
        smooth: true
        color: "white"
    }
}

