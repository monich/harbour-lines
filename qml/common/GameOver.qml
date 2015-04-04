/*
  Copyright (C) 2015 Jolla Ltd.
  Contact: Slava Monich <slava.monich@jolla.com>

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
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
                margins: theme.paddingLarge
            }

            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
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
                if (paintedHeight > 0 && paintedWidth > 0) {
                    if (font.pixelSize % 2) font.pixelSize++
                    while (paintedWidth > width || paintedHeight > height) {
                        if ((font.pixelSize -= 2) <= _minimumSize)
                            break
                    }
                    while (paintedWidth < width && paintedHeight < height) {
                        font.pixelSize += 2
                    }
                    font.pixelSize -= 2
                    if (font.pixelSize >= _maximumSize) {
                        font.pixelSize = _maximumSize
                        return
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
