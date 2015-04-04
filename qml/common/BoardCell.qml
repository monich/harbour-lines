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
    id: cell
    property int row
    property int column
    property string color
    property variant game
    state: "EMPTY"

    property string _lastColor
    property real _size: 1
    property real _touch: 0
    property real _animationDuration: 125

    function stepDone() {
        if (game) game.stepDone(column,row)
    }

/*
    function log(str) {
        console.log("(" + column + "," + row + ") " + str)
    }

    onStateChanged: {
        log(state)
    }
*/

    onColorChanged: {
        //log("color: " + color)
        if (color) _lastColor = color
    }

    Image {
        id: glow
        anchors.fill: parent;
        opacity: _touch
        visible: opacity > 0
        source: "images/glow.svg"
        sourceSize.width: width
        sourceSize.height: height
    }

    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: (ball.opacity && ball.source) ? 1 : 0
        visible: opacity > 0
        source: "images/shadow.svg"
        sourceSize.width: cell.width
        sourceSize.height: cell.height
        width: ball.width
        height: parent.height
    }

    Image {
        id: ball
        opacity: (color || _lastColor) ? 1 : 0
        visible: opacity > 0
        anchors.centerIn: parent;
        source: (color || _lastColor) ? "images/ball-" + (color ? color : _lastColor) + ".svg" : ""
        sourceSize.width: cell.width
        sourceSize.height: cell.height
        width: parent.width * _size
        height: parent.height * _size
    }

    SequentialAnimation {
        id: jumpAnimation
        loops: Animation.Infinite
        running: state == "SELECTED" && globalSystemState.lockMode != "locked"
        NumberAnimation {
            target: cell
            properties: "_size"
            duration: _animationDuration*2
            from: 1
            to: 0.7
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: cell
            properties: "_size"
            duration: _animationDuration*2
            from: 0.7
            to: 1
            easing.type: Easing.InOutQuad
        }
    }

    SequentialAnimation {
        id: touchAnimation
        alwaysRunToEnd: true
        NumberAnimation {
            target: cell
            property: "_touch"
            easing.type: Easing.OutCubic
            duration: _animationDuration/2
            from: 0
            to: 0.3
        }
        NumberAnimation {
            target: cell
            properties: "_touch"
            easing.type: Easing.InCubic
            duration: _animationDuration/2
            from: 0.3
            to: 0
        }
    }

    states: [
        State {
            name: "EMPTY"
            PropertyChanges { target: cell; _size: 0 }
        },
        State {
            name: "STATIC"
            PropertyChanges { target: cell; _size: 1 }
        },
        State {
            name: "SELECTED"
            PropertyChanges { target: cell; _size: 1 }
        },
        State {
            name: "START_MOVE"
            PropertyChanges { target: cell; _size: 0 }
        },
        State {
            name: "MOVING"
            PropertyChanges { target: cell; _size: 0 }
        },
        State {
            name: "END_MOVE"
            PropertyChanges { target: cell; _size: 1 }
        }
    ]

    transitions: [
        Transition {
            to: "START_MOVE"
            SequentialAnimation {
                NumberAnimation {
                    target: cell
                    properties: "_size"
                    from: 1
                    to: 0
                    duration: _animationDuration
                    easing.type: Easing.InOutQuad
                }
                ScriptAction { script: stepDone() }
            }
        },
        Transition {
            to: "MOVING"
            SequentialAnimation {
                NumberAnimation {
                    target: cell
                    properties: "_size"
                    duration: _animationDuration
                    from: 0
                    to: 0.8
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    target: cell
                    properties: "_size"
                    duration: _animationDuration
                    from: 0.8
                    to: 0
                    easing.type: Easing.InOutQuad
                }
                ScriptAction { script: stepDone() }
            }
        },
        Transition {
            to: "END_MOVE"
            SequentialAnimation {
                NumberAnimation {
                    target: cell
                    properties: "_size"
                    duration: _animationDuration
                    from: 0
                    easing.type: Easing.InOutQuad
                }
                ScriptAction { script: stepDone() }
            }
        },
        Transition {
            from: "END_MOVE"
            to: "STATIC"
        },
        Transition {
            from: "EMPTY"
            to: "STATIC"
            NumberAnimation {
                target: cell
                properties: "_size"
                from: 0
                to: 1
                duration: _animationDuration*2
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "NONE"
            to: "STATIC"
            NumberAnimation {
                target: cell
                properties: "_size"
                from: 0
                to: 1
                duration: _animationDuration*2
                easing.type: Easing.InOutQuad
            }
        },
        Transition {
            from: "STATIC"
            to: "EMPTY"
            SequentialAnimation {
                NumberAnimation {
                    target: cell
                    properties: "_size"
                    from: 1
                    duration: _animationDuration*2
                    easing.type: Easing.InOutQuad
                }
                ScriptAction { script: _lastColor = "" }
            }
        },
        Transition {
            from: "END_MOVE"
            to: "EMPTY"
            SequentialAnimation {
                NumberAnimation {
                    target: cell
                    properties: "_size"
                    from: 1
                    duration: _animationDuration*2
                    easing.type: Easing.InOutQuad
                }
                ScriptAction { script: _lastColor = "" }
            }
        },
        Transition {
            to: "EMPTY"
            ScriptAction { script: _lastColor = "" }
        }
    ]

    onClicked: {
        if (game.click(column, row)) {
            touchAnimation.start()
        }
        mouse.accepted = true
    }
}
