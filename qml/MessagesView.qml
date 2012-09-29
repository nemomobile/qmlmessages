/* Copyright (C) 2012 John Brooks <john.brooks@dereferenced.net>
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
import org.nemomobile.qmlmessages 1.0
import org.nemomobile.commhistory 1.0

Item {
    property alias model: view.model
    // The event model is in descending order, but we need to display ascending.
    // There is no sane way to invert the view, but we can use this incredibly
    // bad hack: rotate the view, then rotate the delegate to be upright.
    rotation: 180

    ListView {
        id: view
        spacing: 20
        anchors.fill: parent
        cacheBuffer: parent.height

        // Necessary when opening VKB, for example
        onHeightChanged: view.positionViewAtBeginning()

        Connections {
            target: model || null
            onRowsInserted: {
                if (first == 0)
                    view.positionViewAtBeginning()
            }
            onModelReset: view.positionViewAtBeginning()
        }

        // Cannot use Label, because it shadows the 'style' property.
        // Copy its text formatting instead.
        LabelStyle {
            id: labelStyle
        }

        delegate: BorderImage {
            id: messageBox
            x: model.direction == CommHistory.Outbound ? parent.width - width : 0
            height: childrenRect.height + 20
            width: messageText.paintedWidth + 30
            cache: true
            // Fix rotation from the view hack...
            rotation: 180
            border.left: 24
            border.right: 24
            border.top: 24
            border.bottom: 24

            // This should use meegotouch's speechbubble theme elements, but those SVG group
            // images are not supported in qt-components currently. incoming.svg and outgoing.svg
            // are extracted from the group SVG in meegotouch's base theme and included here.
            source: model.direction == CommHistory.Inbound ? "qrc:/images/incoming.svg" : "qrc:/images/outgoing.svg"

            property int status: model.status

            Text {
                id: messageText
                x: model.direction == CommHistory.Outbound ? 20 : 10
                y: 10
                text: model.freeText
                width: messageBox.parent.width * 0.7
                height: paintedHeight
                wrapMode: Text.Wrap
                style: Text.Raised
                styleColor: "#eeeeee"
                font.family: labelStyle.fontFamily
                font.pixelSize: labelStyle.fontPixelSize
            }
        }
    }

    ScrollDecorator {
        flickableItem: view

        // The rotated view hack screws up ScrollDecorator. This is a (also very bad) workaround.
        anchors.fill: undefined
        anchors.right: view.right
        anchors.rightMargin: view.width - childrenRect.width - 4 - (__hasPageWidth ? __rightPageMargin : 0)
        anchors.top: view.top
        anchors.bottom: view.bottom
    }
}

