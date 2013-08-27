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

import QtQuick 2.0
import com.nokia.meego 2.0
import org.nemomobile.qmlcontacts 1.0

MouseArea {
    id: root

    height: UiConstants.ListItemHeightDefault
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: UiConstants.DefaultMargin
    anchors.rightMargin: UiConstants.DefaultMargin
    opacity: pressed ? 0.7 : 1.0

    property QtObject person: null
    property int contactId: model.contactIds.length > 0 ? model.contactIds[0] : -1

    Connections {
        target: person ? null : peopleModel
        onRowsInserted: updatePerson()
        onModelReset: updatePerson()
    }

    Component.onCompleted: updatePerson()
    onContactIdChanged: updatePerson()

    function updatePerson() {
        person = peopleModel.personById(model.contactIds[0])
    }

    ContactAvatarImage {
        id: photo
        contact: person
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        anchors {
            left: photo.right
            leftMargin: photo.height/4
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        Label {
            id: nameFirst
            width: parent.width - messageDate.paintedWidth
            height: paintedHeight
            elide: Text.ElideRight
            text: person ? person.displayLabel : model.remoteUids[0]

            platformStyle: LabelStyle {
                fontPixelSize: 30
            }

            Label {
                id: messageDate
                // XXX This should be something more natural/useful
                text: Qt.formatDateTime(model.lastModified, "M/d")
                anchors.left: parent.right
                anchors.verticalCenter: parent.verticalCenter

                platformStyle: LabelStyle {
                    fontPixelSize: 22
                    textColor: "#303030"
                }
            }
        }

        Label {
            id: messagePreview
            text: model.lastMessageText
            elide: Text.ElideRight
            width: parent.width
            textFormat: Text.PlainText
            maximumLineCount: 1

            platformStyle: LabelStyle {
                fontPixelSize: 20
                textColor: "#4b4b4b"
            }
        }
    }
}

