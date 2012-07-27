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
        source: (model.avatarPath == undefined) ? "image://theme/icon-m-telephony-contact-avatar" : model.avatarPath
        sourceSize: Qt.size(80, 80)
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        anchors {
            left: photo.right
            leftMargin: photo.height/4
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        Item {
            width: parent.width
            height: nameFirst.paintedHeight

            Label {
                id: nameFirst
                text: model.displayName

                platformStyle: LabelStyle {
                    fontFamily: "Droid Sans"
                    fontPixelSize: 30
                }
            }

            Label {
                id: messageDate
                // XXX This should be something more natural/useful
                text: Qt.formatDateTime(model.messageDate, "M/d")
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                platformStyle: LabelStyle {
                    fontFamily: "Droid Sans"
                    fontPixelSize: 22
                    textColor: "#303030"
                }
            }
        }

        Label {
            id: messagePreview
            text: model.messagePreview
            elide: Text.ElideRight
            width: parent.width

            platformStyle: LabelStyle {
                fontFamily: "Droid Sans"
                fontPixelSize: 20
                textColor: "#4b4b4b"
            }
        }
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

