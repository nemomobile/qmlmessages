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

BorderImage {
    id: textArea
    height: textInput.height + (8*2)
    source: "image://theme/meegotouch-toolbar-portrait-background"
    border { left: 10; right: 10; top: 10; bottom: 10 }

    property alias text: textInput.text

    signal sendMessage

    function clear() {
        text = ""
    }

    TextArea {
        id: textInput
        anchors.left: parent.left
        anchors.leftMargin: UiConstants.ButtonSpacing
        anchors.right: sendBtn.left
        anchors.rightMargin: UiConstants.ButtonSpacing
        y: 8
        height: 52 /* UI.DEFAULT_FIELD.HEIGHT */

        placeholderText: qsTr("Type a message")
        wrapMode: TextEdit.Wrap
        textFormat: TextEdit.PlainText
    }

    Button {
        id: sendBtn
        anchors.right: parent.right
        anchors.rightMargin: UiConstants.ButtonSpacing
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8

        text: qsTr("Send")
        enabled: textInput.text.length > 0

        platformStyle: ButtonStyle {
            buttonWidth: 100
            background: "image://theme/meegotouch-button-inverted-background"
            disabledBackground: background
            textColor: "white"
            disabledTextColor: "lightgray"
        }

        onClicked: sendMessage(text)
    }
}
