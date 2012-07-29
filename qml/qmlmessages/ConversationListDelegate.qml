/* Copyright (C) 2012 John Brooks <john.brooks@dereferenced.net>
 * Copyright (C) 2011 Robin Burchell <robin+nemo@viroteck.net>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

