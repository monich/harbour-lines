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

import QtQuick 1.1
import com.nokia.meego 1.0

MouseArea {
    id: panel

    property variant theme
    property variant prefs

    property real _descriptionMargin: 57

    Image {
        anchors.fill: parent
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
        source: "images/settings-bg.svg"

        Column {
            spacing: theme.paddingLarge
            width: parent.width - 2*theme.paddingLarge
            anchors {
                top: parent.top
                topMargin: theme.paddingLarge
                horizontalCenter: parent.horizontalCenter
            }

            CheckBox {
                text: qsTr("settings-show-next-switch")
                checked: prefs.showNextBalls
                onCheckedChanged: prefs.showNextBalls = checked
            }

            Label {
                anchors {
                    right: parent.right
                    left: parent.left
                    leftMargin: _descriptionMargin
                }
                text: qsTr("settings-show-next-description")
                font.pixelSize: theme.fontSizeTiny
            }

            CheckBox {
                text: qsTr("settings-animate-movement-switch")
                checked: prefs.showBallPath
                onCheckedChanged: prefs.showBallPath = checked
            }

            Label {
                anchors {
                    right: parent.right
                    left: parent.left
                    leftMargin: _descriptionMargin
                }
                text: qsTr("settings-animate-movement-description")
                font.pixelSize: theme.fontSizeTiny
            }
        }
    }

    onClicked: mouse.accepted = true
}
