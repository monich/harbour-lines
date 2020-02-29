/*
  Copyright (C) 2020 Jolla Ltd.
  Copyright (C) 2020 Slava Monich <slava.monich@jolla.com>

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the names of the copyright holders nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

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
import harbour.lines 1.0

Image {
    id: panel

    sourceSize.width: width
    sourceSize.height: height
    fillMode: Image.PreserveAspectFit
    source: "images/settings-bg.svg"

    property variant theme
    property alias game: scoresModel.game

    Component.onCompleted: opacityBehavior.animation = theme.opacityAnimation.createObject(panel)

    ListView {
        id: list

        x: theme.paddingLarge
        y: theme.paddingLarge
        width: parent.width - 2*x
        height: parent.height - 2*y
        clip: true
        model: ScoresModel { id: scoresModel }
        header: Column {
            width: list.width
            Label {
                text: qsTr("scores-title")
                font.pixelSize: theme.fontSizeSmall
                font.bold: true
            }
            Item {
                width: 1
                height: theme.paddingMedium
            }
            Separator {
                width: parent.width - theme.paddingMedium
            }
            Item {
                width: 1
                height: theme.paddingMedium
            }
        }
        delegate: Item {
            height: Math.max(scoreLabel.height, arrowLabel.height, currentScoreLabel.height, dateAndTimeLabel.height) + theme.paddingSmall
            width: list.width
            property real xSplit: Math.round(parent.width/4)
            Label {
                id: scoreLabel

                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: theme.fontSizeMedium
                font.bold: currentScore
                text: score
            }
            Label {
                id: arrowLabel

                x: xSplit - width - theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: theme.fontSizeSmall
                font.bold: true
                visible: currentScore
                text: "\u2190" // Left arrow
            }
            Label {
                id: currentScoreLabel

                x: xSplit
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: theme.fontSizeSmall
                font.bold: true
                visible: currentScore
                text: qsTr("scores-current-score")
            }
            Column {
                id: dateAndTimeLabel

                x: xSplit
                width: parent.width - x
                anchors.verticalCenter: parent.verticalCenter
                visible: !currentScore
                Label {
                    width: parent.width
                    font.pixelSize: theme.fontSizeExtraSmall
                    text: timestamp.toLocaleDateString()
                }
                Label {
                    width: parent.width
                    font.pixelSize: theme.fontSizeTiny
                    text: timestamp.toLocaleTimeString()
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: list
    }

    Behavior on opacity { id: opacityBehavior }
}
