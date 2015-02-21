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

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: window.allowedOrientations
    property var game

    property bool _portrait: page.orientation === Orientation.Portrait
    property string _highScore: (game && game.highScore) ? game.highScore : ""
    property bool _firstScore: true
    property int _displayedScore: 0
    property int score: game ? game.score : 0

    onScoreChanged: {
        if (_firstScore) {
            _firstScore = false
            _displayedScore = score
        } else if (_displayedScore > score) {
            _displayedScore = score
        }
    }

    Timer {
        running: _displayedScore < score
        interval: 50
        repeat: true
        onTriggered: if (_displayedScore < score) _displayedScore += 1
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("menu-new-game")
                onClicked: game.restart()
            }
        }

        Board {
            id: board
            x: _portrait ? Theme.paddingLarge : (parent.width - width)/2
            y: _portrait ? (Theme.paddingLarge + scoreItem.height + (parent.height - height - Theme.paddingLarge - scoreItem.height)/2) : Theme.paddingLarge
            width: (_portrait ? page.width : parent.height) - 2*Theme.paddingLarge
            height: width
            game: page.game
        }

        Score {
            id: scoreItem
            text: _displayedScore
            anchors {
                bottomMargin: Theme.paddingLarge
                rightMargin: Theme.paddingLarge
            }
            horizontalAlignment: _portrait ? Text.AlignLeft : Text.AlignRight
        }

        Score {
            id: highScoreItem
            text: _highScore
            opacity: 0.5
            anchors {
                leftMargin: Theme.paddingLarge
                bottomMargin: Theme.paddingLarge
            }
            horizontalAlignment: _portrait ? Text.AlignRight : Text.AlignLeft
        }

        PulleyAnimationHint {
            id: pulleyAnimationHint
            anchors.fill: parent
            enabled: game.over
        }

        states: [
            State {
                name: "PORTRAIT"
                when:  _portrait
                AnchorChanges {
                    target: scoreItem
                    anchors.left: board.left
                    anchors.bottom: board.top
                    anchors.top: undefined
                    anchors.right: undefined
                }
                AnchorChanges {
                    target: highScoreItem
                    anchors.right: board.right
                    anchors.bottom: board.top
                    anchors.top: undefined
                    anchors.left: undefined
                }
            },
            State {
                name: "LANDSCAPE"
                when: !_portrait
                AnchorChanges {
                    target: scoreItem
                    anchors.left: undefined
                    anchors.bottom: undefined
                    anchors.top: board.top
                    anchors.right: board.left
                }
                AnchorChanges {
                    target: highScoreItem
                    anchors.right: undefined
                    anchors.bottom: undefined
                    anchors.top: board.top
                    anchors.left: board.right
                }
            }
        ]
    }
}
