/*
  Copyright (C) 2015-2016 Jolla Ltd.
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

import QtQuick 1.1
import com.nokia.meego 1.0

MouseArea {
    id: panel

    property variant theme
    property variant prefs

    Image {
        anchors.fill: parent
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
        source: "images/settings-bg.svg"

        Rectangle {
            x: 0
            y: 0
            width: parent.width
            height: column.y + stylePopup.y + stylePopup.height + theme.paddingLarge/2
            color: "black"
            opacity: stylePopup.pressed ? 0.2 : 0
            Behavior on opacity { id: styleButtonOpacityBehavior }
        }

        Column {
            id: column
            spacing: theme.paddingLarge
            width: parent.width - 2*theme.paddingLarge
            anchors {
                top: parent.top
                topMargin: theme.paddingLarge
                horizontalCenter: parent.horizontalCenter
            }

            PopupButton {
                id: stylePopup
                width: parent.width
                theme: panel.theme
                label: qsTr("settings-style")
                onClicked: styleSelection.open()
            }

            TextSwitch {
                theme: panel.theme
                width: parent.width
                text: qsTr("settings-show-next-switch")
                description: qsTr("settings-show-next-description")
                checked: prefs.showNextBalls
                onCheckedChanged: prefs.showNextBalls = checked
            }

            TextSwitch {
                theme: panel.theme
                width: parent.width
                text: qsTr("settings-animate-movement-switch")
                description: qsTr("settings-animate-movement-description")
                checked: prefs.showBallPath
                onCheckedChanged: prefs.showBallPath = checked
            }
        }
    }

    SelectionDialog {
        id: styleSelection
        titleText: qsTr("settings-style")
        model: styleModel
        onSelectedIndexChanged: {
            if (selectedIndex >= 0) {
                var selectedItem = model.get(selectedIndex)
                stylePopup.value = selectedItem.name
                // First make sure that shadow is hidden. The style
                // may have no shadow, and switching the style first
                // may result in attempt to open non-existent image.
                prefs.showBallShadow = false
                prefs.ballStyle = selectedItem.style
                prefs.showBallShadow = selectedItem.shadow
            }
        }
    }

    ListModel {
        id: styleModel
        ListElement {
            trId: "settings-style-ball"
            name: ""
            style: "ball"
            shadow: true
        }
        ListElement {
            trId: "settings-style-shape"
            name: ""
            style: "shape"
            shadow: false
        }
    }

    Component.onCompleted: {
        for (var i=0; i<styleModel.count; i++) {
            var item = styleModel.get(i)
            var name = qsTr(item.trId)
            styleModel.setProperty(i, "name", name)
            if (item.style === prefs.ballStyle) {
                styleSelection.selectedIndex = i
            }
        }
        styleButtonOpacityBehavior.animation = theme.opacityAnimation.createObject(panel)
    }

    onClicked: mouse.accepted = true
}
