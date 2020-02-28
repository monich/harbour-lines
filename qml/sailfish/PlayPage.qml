/*
  Copyright (C) 2015-2020 Jolla Ltd.
  Copyright (C) 2015-2020 Slava Monich <slava.monich@jolla.com>

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

import "../common"
import "../harbour"

Page {
    id: page
    allowedOrientations: window.allowedOrientations
    property variant game
    property variant theme

    property bool _settingsMode
    property bool _portrait: page.orientation === Orientation.Portrait
    property string _highScore: (game && game.highScore) ? game.highScore : ""
    property int _score: game ? game.score : 0
    property string _ballStyle: (game && game.prefs) ? game.prefs.ballStyle : "ball"
    property real cellSize: _portrait ?
        Math.min((width - 2*theme.paddingLarge)/Lines.Columns,
                 (height - 4*theme.paddingLarge)/(Lines.Rows+2)) :
        Math.min((height - 2*theme.paddingLarge)/Lines.Rows,
                 (width - 4*theme.paddingLarge)/(Lines.Columns+2))

    NextBallsModel {
        id: nextBallsModel
        game: page.game
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
            theme: page.theme
            anchors.centerIn: parent
            cellSize: page.cellSize
            width: cellSize * Lines.Columns
            height: cellSize * Lines.Rows
            game: page.game
            opacity: _settingsMode ? 0 : 1
            visible: opacity > 0
            source: HarbourTheme.darkOnLight ? Qt.resolvedUrl("images/board-light.svg") : Qt.resolvedUrl("../common/images/board.svg")
            Behavior on opacity { FadeAnimation {} }
        }

        GridView {
            id: nextBalls
            width: cellSize * (_portrait ? nextBallsModel.count : 1)
            height: cellSize * (_portrait ? 1 : nextBallsModel.count)
            anchors {
                leftMargin: _portrait ? 0 : theme.paddingLarge
                bottomMargin: _portrait ? theme.paddingLarge : 0
            }
            opacity: game && game.prefs && game.prefs.showNextBalls ? 1 : 0
            cellWidth: cellSize
            cellHeight: cellSize
            model: nextBallsModel
            delegate: NextBall {
                width: cellSize
                height: cellSize
                color: model.color
                style: page._ballStyle
                stateIndex: nextBallsModel.stateIndex
            }
            visible: opacity > 0
            Behavior on opacity { FadeAnimation {} }
        }

        Label {
            id: scoreLabel
            anchors.left: scoreItem.left
            text: qsTr("label-score")
            font.pixelSize: Theme.fontSizeExtraSmall
            color: scoreItem.color
            opacity: 0.4
        }

        Score {
            id: scoreItem
            horizontalAlignment: Text.AlignLeft
            anchors {
                bottomMargin: theme.paddingLarge
                rightMargin: theme.paddingLarge
                leftMargin: _portrait ? 0 : theme.paddingLarge
            }
            transform: HarbourTextFlip {
                text: _score
                target: scoreItem
                readonly property bool flipping: animation.running
                onFlippingChanged: if (!flipping && _score > _highScore) yey.start()
            }
            SequentialAnimation {
                id: yey
                alwaysRunToEnd: true
                NumberAnimation {
                    to: 1.4
                    property: "scale"
                    target: scoreItem
                    duration: 75
                }
                NumberAnimation {
                    to: 1.0
                    property: "scale"
                    target: scoreItem
                    duration: 75
                }
            }
        }

        Label {
            id: highScoreLabel
            anchors.right: highScoreItem.right
            text: qsTr("label-high-score")
            font.pixelSize: Theme.fontSizeExtraSmall
            color: highScoreItem.color
            opacity: 0.4
        }

        Score {
            id: highScoreItem
            text: _highScore
            color: Theme.secondaryColor
            horizontalAlignment: Text.AlignRight
            anchors {
                bottomMargin: theme.paddingLarge
                leftMargin: theme.paddingLarge
                rightMargin: _portrait ? 0 : theme.paddingLarge
            }
            transform: HarbourTextFlip {
                text: _highScore
                target: highScoreItem
            }
        }

        PulleyAnimationHint {
            id: pulleyAnimationHint
            anchors.fill: parent
            enabled: game.over
        }

        SettingsPanel {
            prefs: game.prefs
            x: board.x
            y: board.y
            width: board.width
            height: board.height
            visible: opacity > 0
            opacity: _settingsMode ? 1 : 0
            Behavior on opacity { FadeAnimation {} }
        }

        SettingsButton {
            theme: page.theme
            width: cellSize
            height: width
            ok: _settingsMode
            imagePrefix: "image://" + HarbourImageProvider + "/"
            imageSuffix: "?" + Theme.primaryColor
            anchors {
                bottom: parent.bottom
                right: parent.right
                margins: theme.paddingLarge
            }
            onButtonClicked: _settingsMode = !_settingsMode
        }

        states: [
            State {
                name: "PORTRAIT"
                when:  _portrait
                AnchorChanges {
                    target: scoreLabel
                    anchors {
                        top: undefined
                        bottom: scoreItem.top
                    }
                }
                AnchorChanges {
                    target: scoreItem
                    anchors {
                        top: undefined
                        left: board.left
                        bottom: board.top
                        right: undefined
                    }
                }
                AnchorChanges {
                    target: highScoreLabel
                    anchors {
                        top: undefined
                        bottom: highScoreItem.top
                    }
                }
                AnchorChanges {
                    target: highScoreItem
                    anchors {
                        top: undefined
                        left: undefined
                        bottom: board.top
                        right: board.right
                    }
                }
                AnchorChanges {
                    target: nextBalls
                    anchors {
                        left: board.left
                        bottom: parent.bottom
                    }
                }
            },
            State {
                name: "LANDSCAPE"
                when: !_portrait
                AnchorChanges {
                    target: scoreLabel
                    anchors {
                        top: scoreItem.bottom
                        bottom: undefined
                    }
                }
                AnchorChanges {
                    target: scoreItem
                    anchors {
                        top: board.top
                        left: parent.left
                        bottom: undefined
                        right: board.left
                    }
                }
                AnchorChanges {
                    target: highScoreLabel
                    anchors {
                        top: highScoreItem.bottom
                        bottom: undefined
                    }
                }
                AnchorChanges {
                    target: highScoreItem
                    anchors {
                        top: board.top
                        left: board.right
                        right: parent.right
                        bottom: undefined
                    }
                }
                AnchorChanges {
                    target: nextBalls
                    anchors {
                        left: parent.left
                        bottom: board.bottom
                    }
                }
            }
        ]
    }
}
