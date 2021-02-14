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

Rectangle {
    id: panel

    radius: width/22
    color: "transparent"
    border {
        width: Math.max(1, Math.ceil(width/150))
        color: Theme.primaryColor
    }

    property var prefs

    Component.onCompleted: styleComboBox.selectCurrentStyle()

    Connections {
        target: prefs
        onBallStyleChanged: styleComboBox.selectCurrentStyle()
    }

    SilicaFlickable {
        id: scrollArea

        x: parent.border.width + Theme.paddingLarge
        y: parent.border.width + Theme.paddingLarge
        width: parent.width - 2*x
        height: parent.height - 2*y
        contentHeight: content.height
        clip: true

        Column {
            id: content

            width: parent.width

            ComboBox {
                id: styleComboBox

                function selectCurrentStyle() {
                    if (prefs && prefs.ballStyle) {
                        var style = prefs.ballStyle
                        var items = styleMenu.children
                        if (items) {
                            for (var i=0; i<items.length; i++) {
                                if (items[i].style === style) {
                                    currentItem = items[i]
                                    return
                                }
                            }
                        }
                    }
                    currentItem = null
                }

                function applyStyle(style,shadow) {
                    // First make sure that shadow is hidden. The style
                    // may have no shadow, and switching the style first
                    // may result in attempt to open non-existent image.
                    prefs.showBallShadow = false
                    prefs.ballStyle = style
                    prefs.showBallShadow = shadow
                }

                width: parent.width
                label: qsTr("settings-style")
                menu: ContextMenu {
                    id: styleMenu

                    x: 0
                    MenuItem {
                        text: qsTr("settings-style-ball")
                        readonly property string style: "ball"
                        readonly property bool shadow: true
                        onClicked: styleComboBox.applyStyle(style, shadow)
                    }
                    MenuItem {
                        text: qsTr("settings-style-shape")
                        readonly property string style: "shape"
                        readonly property bool shadow: false
                        onClicked: styleComboBox.applyStyle(style, shadow)
                    }
                }
            }
            TextSwitch {
                text: qsTr("settings-play-sounds-switch")
                automaticCheck: false
                checked: prefs.playSounds
                onClicked: prefs.playSounds = !checked
            }
            TextSwitch {
                text: qsTr("settings-show-next-switch")
                description: qsTr("settings-show-next-description")
                automaticCheck: false
                checked: prefs.showNextBalls
                onClicked: prefs.showNextBalls = !checked
            }
            TextSwitch {
                text: qsTr("settings-animate-movement-switch")
                description: qsTr("settings-animate-movement-description")
                automaticCheck: false
                checked: prefs.showBallPath
                onClicked: prefs.showBallPath = !checked
            }
        }
    }

    // Reparent scroll decorator so that its bottom part is not affected
    // by the opacity ramp effect
    Item {
        id: scrollDecoratorItem

        width: scrollDecorator.width
        anchors {
            top: scrollArea.top
            bottom: scrollArea.bottom
            right: scrollArea.right
        }

        VerticalScrollDecorator {
            id: scrollDecorator

            flickable: scrollArea
            onFlickableChanged: parent = scrollDecoratorItem
        }
    }

    // Somehow opacity ramp effect interferes with page dimming when
    // context menu is active
    OpacityRampEffect {
        enabled: !styleMenu.active && !scrollArea.atYEnd
        sourceItem: scrollArea
        slope: scrollArea.height/Theme.paddingLarge
        offset: 1 - Theme.paddingLarge/scrollArea.height
        direction: OpacityRamp.TopToBottom
    }
}
