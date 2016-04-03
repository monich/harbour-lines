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

MouseArea {
    id: panel

    property var prefs

    Component.onCompleted: styleComboBox.updateCurrentStyle()

    Connections {
        target: prefs
        onBallStyleChanged: styleComboBox.updateCurrentStyle()
    }

    Image {
        anchors.fill: parent
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
        source: "images/settings-bg.svg"
    }

    SilicaFlickable {
        x: Theme.paddingLarge
        width: parent.width - 2*Theme.paddingLarge
        height: parent.height
        anchors.centerIn: parent
        contentHeight: content.height
        clip: true

        Column {
            id: content
            width: parent.width
            anchors {
                top: parent.top
                topMargin: Theme.paddingLarge
                horizontalCenter: parent.horizontalCenter
            }
            ComboBox {
                id: styleComboBox
                property bool _externalChange: false

                function updateCurrentStyle() {
                    var savedFlag = styleComboBox._externalChange
                    styleComboBox._externalChange = true
                    if (prefs && prefs.ballStyle) {
                        var itemFound = null
                        var style = prefs.ballStyle
                        var items = styleMenu.children
                        if (items) {
                            for (var i=0; i<items.length; i++) {
                                if (items[i].style === style) {
                                    itemFound = items[i]
                                    break;
                                }
                            }
                        }
                        if (itemFound) {
                            currentItem = itemFound
                        } else {
                            currentItem = null
                        }
                    } else {
                        currentItem = null
                    }
                    styleComboBox._externalChange = savedFlag
                }

                onCurrentIndexChanged: {
                    if (!styleComboBox._externalChange && currentItem) {
                        // First make sure that shadow is hidden. The style
                        // may have no shadow, and switching the style first
                        // may result in attempt to open non-existent image.
                        prefs.showBallShadow = false
                        prefs.ballStyle = currentItem.style
                        prefs.showBallShadow = currentItem.shadow
                    }
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
                    }
                    MenuItem {
                        text: qsTr("settings-style-shape")
                        readonly property string style: "shape"
                        readonly property bool shadow: false
                    }
                }
            }
            TextSwitch {
                text: qsTr("settings-show-next-switch")
                description: qsTr("settings-show-next-description")
                checked: prefs.showNextBalls
                onCheckedChanged: prefs.showNextBalls = checked
            }
            TextSwitch {
                text: qsTr("settings-animate-movement-switch")
                description: qsTr("settings-animate-movement-description")
                checked: prefs.showBallPath
                onCheckedChanged: prefs.showBallPath = checked
            }
        }
    }

    onClicked: mouse.accepted = true
}
