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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.lines 1.0
import "../common"

CoverBackground {
    id: cover

    property var game
    property alias theme: board.theme
    property alias boardBackground: board.source
    readonly property bool newRecord: game && game.newRecord
    readonly property real cellSize: Math.floor((width - 2*Theme.paddingLarge)/Lines.Columns)

    Image {
        id: icon

        anchors {
            bottom: board.top
            bottomMargin: Theme.paddingLarge
            right: parent.right
            rightMargin: Theme.paddingLarge
        }
        height: Math.min(Theme.fontSizeHuge, 3*cellSize)
        width: height * implicitWidth / implicitHeight
        source: Qt.resolvedUrl("images/" + (newRecord ? "king.png" : "pretender.png"))
        smooth: true
    }

    Label {
        id: score

        anchors {
            top: parent.top
            topMargin: Theme.paddingMedium
            left: parent.left
            leftMargin: Theme.paddingLarge
            right: icon.left
            rightMargin: Theme.paddingLarge
        }
        text: game.score
        color: Theme.primaryColor
        minimumPixelSize: Theme.fontSizeSmall
        fontSizeMode: Text.Fit
        font {
            family: Theme.fontFamilyHeading
            pixelSize: Theme.fontSizeHuge
       }
    }

    Board {
        id: board

        theme: cover.theme
        anchors {
            top: score.bottom
            topMargin: Theme.paddingMedium
            horizontalCenter: parent.horizontalCenter
        }
        animateSelection: false
        cellSize: cover.cellSize
        width: cellSize * Lines.Columns
        height: cellSize * Lines.Rows
        game: cover.game
        visible: opacity > 0
    }

    Image {
        readonly property real horizontalMargins: Theme.paddingLarge
        readonly property real verticalMargins: Theme.paddingMedium
        anchors {
            top: board.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            topMargin: verticalMargins
            bottomMargin: verticalMargins
            leftMargin: horizontalMargins
            rightMargin: horizontalMargins
        }
        fillMode: Image.PreserveAspectFit
        source: Qt.resolvedUrl("images/lines.png")
        smooth: true
    }
}
