/*
  Copyright (C) 2015-2021 Jolla Ltd.
  Copyright (C) 2015-2021 Slava Monich <slava.monich@jolla.com>

  You may use this file under the terms of the BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer
       in the documentation and/or other materials provided with the
       distribution.
    3. Neither the names of the copyright holders nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//import QtQuick 1.1  // Harmattan
import QtQuick 2.0  // Sailfish

MouseArea {
    id: panel
    property variant theme
    property alias text: textComponent.text

    signal panelClicked()

    Rectangle {
        color: "#B0000000"
        anchors.fill: parent

        Text {
            id: textComponent
            anchors {
                fill: parent
                margins: Math.floor(Math.min(theme.paddingLarge, parent.height/5))
            }

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "white"
            smooth: true
            font.pixelSize: theme.fontSizeHuge
            font.bold: true

            onWidthChanged: refitText()
            onHeightChanged: refitText()
            onTextChanged: refitText()

            property int _minimumSize: 8
            property int _maximumSize: 200

            function refitText() {
                if (implicitHeight > 0 && implicitWidth > 0) {
                    if (font.pixelSize % 2) font.pixelSize++
                    while ((implicitWidth > width || implicitHeight > height || font.pixelSize > _maximumSize) && (font.pixelSize - 2) >= _minimumSize) {
                        font.pixelSize -= 2
                    }
                    while (implicitWidth < width && implicitHeight < height && (font.pixelSize + 2) <= _maximumSize) {
                        font.pixelSize += 2
                    }
                    if (implicitWidth > width || implicitHeight > height) {
                        font.pixelSize -= 2
                    }
                }
            }
        }
    }

    onClicked: {
        mouse.accepted = true
        panel.panelClicked()
    }
}
