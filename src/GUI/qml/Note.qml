// SPDX-FileCopyrightText: 2025 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.4
import FlightGear 1.0

Rectangle {
    property alias text: message.text

    height: message.height + (Style.margin * 2)

    border.width: 1
    border.color: Style.noteFrameColor
    color: Style.noteBackgroundColor

    Text {
        id: message

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Style.margin
        }

        font.pixelSize: Style.baseFontPixelSize
        color: Style.baseTextColor
    }
}
