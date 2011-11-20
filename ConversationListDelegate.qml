/*
 * Copyright 2011 Robin Burchell
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: root

    height: photo.height + itemMargins
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: itemMargins
    anchors.rightMargin: itemMargins

    property int itemMargins: 10

    signal clicked

    Image {
        id: photo
        source: model.personavatarPath
        anchors.verticalCenter: parent.verticalCenter
    }

    Label {
        id: nameFirst
        text: model.persondisplayLabel
        anchors {
            left: photo.right;
            verticalCenter: parent.verticalCenter;
            leftMargin: photo.height/8
        }
        smooth: true
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            parent.clicked();
        }
    }

    states: State {
        name: "pressed"; when: mouseArea.pressed == true
        PropertyChanges { target: root; opacity: .7}
    }

}

