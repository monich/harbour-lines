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

import QtQuick 1.1  // Harmattan
//import QtQuick 2.0  // Sailfish

Item {
    id: ball
    property string color
    property int stateIndex

    property string _displayedColor
    property real _size: 1
    property real _animationDuration: 125

    onColorChanged: {
        if (_displayedColor) {
            if (color) {
                colorChangeAnimation.start()
            } else {
                disappearAnimation.start()
            }
        } else if (color) {
            appearAnimation.start()
        }
    }

    onStateIndexChanged: {
        if (_displayedColor) {
            colorChangeAnimation.start()
        }
    }

    Image {
        opacity: _displayedColor ? 1 : 0
        visible: opacity > 0
        anchors.centerIn: parent;
        source: _displayedColor ? "images/ball-" + _displayedColor + ".svg" : ""
        sourceSize.width: ball.width
        sourceSize.height: ball.height
        width: parent.width * _size
        height: parent.height * _size
    }

    SequentialAnimation {
        id: disappearAnimation
        NumberAnimation {
            target: ball
            properties: "_size"
            from: 1
            to: 0
            duration: _animationDuration
            easing.type: Easing.InOutQuad
        }
        ScriptAction { script: _displayedColor = color }
    }

    SequentialAnimation {
        id: appearAnimation
        ScriptAction { script: _displayedColor = color }
        NumberAnimation {
            target: ball
            properties: "_size"
            duration: _animationDuration
            from: 0
            to: 1
            easing.type: Easing.InOutQuad
        }
    }

    SequentialAnimation {
        id: colorChangeAnimation
        NumberAnimation {
            target: ball
            properties: "_size"
            from: 1
            to: 0
            duration: _animationDuration
            easing.type: Easing.InOutQuad
        }
        ScriptAction { script: _displayedColor = color }
        NumberAnimation {
            target: ball
            properties: "_size"
            duration: _animationDuration
            from: 0
            to: 1
            easing.type: Easing.InOutQuad
        }
    }
}
