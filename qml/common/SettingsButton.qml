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

//import QtQuick 1.1  // Harmattan
import QtQuick 2.0  // Sailfish

HighlightableMouseArea {
    id: button

    property bool ok
    property string imagePrefix
    property string imageSuffix

    signal buttonClicked()

    Component.onCompleted: {
        settingsOpacityBehavior.animation = theme.opacityAnimation.createObject(settingsImage)
        okOpacityBehavior.animation = theme.opacityAnimation.createObject(okImage)
    }

    Image {
        id: settingsImage
        anchors.fill: parent
        source: imagePrefix + Qt.resolvedUrl("images/settings-normal.svg") + imageSuffix
        sourceSize.width: width
        sourceSize.height: height
        visible: opacity > 0
        opacity: ok ? 0 : 1
        Behavior on opacity { id: settingsOpacityBehavior }
    }

    Image {
        id: okImage
        anchors.fill: parent;
        source: imagePrefix + Qt.resolvedUrl("images/settings-ok.svg") + imageSuffix
        sourceSize.width: width
        sourceSize.height: height
        visible: opacity > 0
        opacity: ok ? 1 : 0
        Behavior on opacity { id: okOpacityBehavior }
    }

    onClicked: {
        mouse.accepted = true
        button.buttonClicked()
    }
}
