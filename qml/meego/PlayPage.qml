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

import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.lines 1.0
import "../common"

Item {
    id: page
    property variant game
    property variant theme

    property int kModeGame: 0
    property int kModeScores: 1
    property int kModeSettings: 2
    property int _mode: kModeGame

    property bool _portrait: window.inPortrait
    property string _highScore: (game && game.highScore) ? game.highScore : ""
    property bool _firstScore: true
    property int _displayedScore: 0
    property int score: game ? game.score : 0
    property string _ballStyle: (game && game.prefs) ? game.prefs.ballStyle : "ball"
    property real cellSize

    function updateCellSize() {
        cellSize = Math.floor((height > width) ?
            Math.min((width - 2*theme.paddingLarge)/Lines.Columns,
                     (height - 4*theme.paddingLarge)/(Lines.Rows+2)) :
            Math.min((height - 2*theme.paddingLarge)/Lines.Rows,
                     (width - 4*theme.paddingLarge)/(Lines.Columns+2)))
    }

    Component.onCompleted: {
        boardOpacityBehavior.animation = theme.opacityAnimation.createObject(board)
        nextBallsOpacityBehavior.animation = theme.opacityAnimation.createObject(nextBalls)
        settingsPanelOpacityBehavior.animation = theme.opacityAnimation.createObject(settingsPanel)
        updateCellSize()
    }

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

    NextBallsModel {
        id: nextBallsModel
        game: page.game
    }

    QueryDialog {
        id: newGameDialog
        icon: "images/logo.svg"
        message: qsTr("new-game-question")
        acceptButtonText: qsTr("new-game-accept")
        rejectButtonText: qsTr("new-game-reject")
        onAccepted: game.restart()
    }

    Rectangle {
        id: background
        color: "black"
        anchors.fill: parent

        Board {
            id: board
            theme: page.theme
            anchors.centerIn: parent
            cellSize: page.cellSize
            width: cellSize * Lines.Columns
            height: cellSize * Lines.Rows
            game: page.game
            opacity: (_mode === kModeGame) ? 1 : 0
            visible: opacity > 0
            Behavior on opacity { id: boardOpacityBehavior }
            onGameOverPanelClicked: newGameDialog.open()
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
            Behavior on opacity { id: nextBallsOpacityBehavior }
        }

        Label {
            id: scoreLabel
            anchors.left: scoreItem.left
            text: qsTr("label-score")
            font.pixelSize: theme.fontSizeExtraSmall
            color: scoreItemText.color
            opacity: 0.4
        }

        MouseArea {
            id: scoreItem
            width: scoreItemText.implicitWidth
            height: scoreItemText.implicitHeight
            anchors {
                bottomMargin: theme.paddingLarge
                rightMargin: theme.paddingLarge
                leftMargin: _portrait ? 0 : theme.paddingLarge
            }
            Score {
                id: scoreItemText
                theme: page.theme
                text: _displayedScore
                horizontalAlignment: Text.AlignLeft
            }
            onClicked: if (_mode === kModeGame) newGameDialog.open()
        }

        Label {
            id: highScoreLabel
            anchors.right: highScoreItem.right
            text: qsTr("label-high-score")
            font.pixelSize: theme.fontSizeExtraSmall
            color: highScoreItem.color
            opacity: 0.4
            property real xright: x + width
            property real ybottom: y + height
        }

        Score {
            id: highScoreItem
            theme: page.theme
            text: _highScore
            opacity: 0.5
            horizontalAlignment: Text.AlignRight
            anchors {
                bottomMargin: theme.paddingLarge
                leftMargin: theme.paddingLarge
                rightMargin: _portrait ? 0 : theme.paddingLarge
            }
            property real xright: x + width
            property real ybottom: y + height
        }

        MouseArea {
            id: highScoreMouseArea
            x: Math.min(highScoreItem.x, highScoreLabel.x)
            y: Math.min(highScoreItem.y, highScoreLabel.y)
            width: xright - x
            height: ybottom - y
            visible: highScoreItem.visible
            property real xright: Math.max(highScoreItem.xright, highScoreLabel.xright)
            property real ybottom: Math.max(highScoreItem.ybottom, highScoreLabel.ybottom)
            onClicked: _mode = (_mode === kModeScores) ? kModeGame : kModeScores
        }

        SettingsPanel {
            id: settingsPanel
            theme: page.theme
            prefs: game.prefs
            x: board.x
            y: board.y
            width: board.width
            height: board.height
            visible: opacity > 0
            opacity: (_mode === kModeSettings) ? 1 : 0
            Behavior on opacity { id: settingsPanelOpacityBehavior }
        }

        ScoresPanel {
            game: page.game
            theme: page.theme
            anchors.fill: board
            visible: opacity > 0
            opacity: (_mode === kModeScores) ? 1 : 0
        }

        SettingsButton {
            theme: page.theme
            width: cellSize
            height: width
            ok: _mode !== kModeGame
            anchors {
                bottom: parent.bottom
                right: parent.right
                margins: theme.paddingLarge
            }
            onButtonClicked: _mode = (ok ? kModeGame : kModeSettings)
        }
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
