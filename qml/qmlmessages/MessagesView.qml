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

Item {
    property alias model: view.model

    ListView {
        id: view
        spacing: 20
        anchors.fill: parent
        cacheBuffer: parent.height

        onCountChanged: view.positionViewAtEnd()
        // Necessary when opening VKB, for example
        onHeightChanged: view.positionViewAtEnd()

        delegate: BorderImage {
            id: messageBox
            height: childrenRect.height + 20
            width: childrenRect.width + 30
            cache: true

            property int status: model.status

            Item {
                id: messageContent
                height: messageText.paintedHeight
                width: messageText.paintedWidth
                y: 10
                x: 10

                Text {
                    id: messageText
                    text: model.text
                    width: messageBox.parent.width * 0.7
                    wrapMode: Text.Wrap
                    style: Text.Raised
                    styleColor: "#eeeeee"
                    font.family: labelStyle.fontFamily
                    font.pixelSize: labelStyle.fontPixelSize

                    // Cannot use Label, because it shadows the 'style' property.
                    // Copy its text formatting instead.
                    LabelStyle {
                        id: labelStyle
                    }
                }
            }

            function showError(msg) {
                errorComponent.createObject(messageBox, { text: msg })
            }
            onStatusChanged: {
                if (status < 0)
                    showError("Failed to deliver message")
            }

            // This should use meegotouch's speechbubble theme elements, but those SVG group
            // images are not supported in qt-components currently. incoming.svg and outgoing.svg
            // are extracted from the group SVG in meegotouch's base theme and included here.
            states: [
                State {
                    name: "incoming"
                    when: model.direction == ChatModel.Incoming

                    PropertyChanges {
                        target: messageBox
                        x: parent.width - width
                        source: "qrc:/images/incoming.svg"
                        border.left: 24
                        border.right: 24
                        border.top: 24
                        border.bottom: 24
                    }
                },
                State {
                    name: "outgoing"
                    when: model.direction == ChatModel.Outgoing

                    PropertyChanges {
                        target: messageBox
                        source: "qrc:/images/outgoing.svg"
                        border.left: 24
                        border.right: 24
                        border.top: 24
                        border.bottom: 24
                    }

                    PropertyChanges {
                        target: messageContent
                        x: 20
                    }
                }
            ]

            Component {
                id: errorComponent

                Item {
                    id: errorContent
                    anchors.top: messageContent.bottom
                    anchors.topMargin: 5
                    anchors.left: messageContent.left
                    width: Math.max(errorText.paintedWidth, messageText.paintedWidth)
                    height: errorText.paintedHeight + 6

                    property alias text: errorText.text

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 1
                        color: "#afafaf"
                    }

                    Label {
                        id: errorText
                        width: messageText.width
                        anchors.top: parent.top
                        anchors.topMargin: 5

                        platformStyle: LabelStyle {
                            textColor: "red"
                        }
                    }
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: view
    }
}

