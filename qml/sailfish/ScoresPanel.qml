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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.lines 1.0

Rectangle {
    id: panel

    radius: width/22
    color: "transparent"
    border {
        width: Math.max(1, Math.ceil(width/150))
        color: Theme.primaryColor
    }

    property alias game: scoresModel.game

    SilicaListView {
        id: list

        x: Theme.paddingLarge + Theme.horizontalPageMargin
        y: parent.border.width + Theme.paddingLarge
        width: parent.width - 2*Theme.paddingLarge - Theme.horizontalPageMargin
        height: parent.height - 2*y
        clip: true
        model: ScoresModel { id: scoresModel }
        header: Column {
            width: list.width
            Label {
                text: qsTr("scores-title")
            }
            Item {
                width: 1
                height: Theme.paddingMedium
            }
            Separator {
                width: parent.width
                color: Theme.primaryColor
            }
            Item {
                width: 1
                height: Theme.paddingMedium
            }
        }
        delegate: Item {
            height: Math.max(scoreLabel.height, arrowLabel.height, currentScoreLabel.height, dateAndTimeLabel.height) + Theme.paddingMedium
            width: list.width
            readonly property real xSplit: Math.round(parent.width/4)
            Label {
                id: scoreLabel

                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Theme.fontSizeLarge
                font.bold: currentScore
                color: currentScore ? currentScoreLabel.color : Theme.primaryColor
                text: score
            }
            Label {
                id: arrowLabel

                x: xSplit - width - Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Theme.fontSizeSmall
                color: currentScoreLabel.color
                visible: currentScore
                text: "\u2190" // Left arrow
            }
            Label {
                id: currentScoreLabel

                x: xSplit
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
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
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                    text: timestamp.toLocaleDateString(Qt.locale())
                }
                Label {
                    width: parent.width
                    font.pixelSize: Theme.fontSizeTiny
                    truncationMode: TruncationMode.Fade
                    text: timestamp.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                }
            }
        }
        VerticalScrollDecorator { }
    }
}
