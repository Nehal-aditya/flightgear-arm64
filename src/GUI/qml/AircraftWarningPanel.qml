// SPDX-FileCopyrightText: 2018 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.4
import FlightGear.Launcher 1.0

Rectangle {
    id: root
    property int aircraftStatus

    visible: (aircraftStatus != LocalAircraftCache.AircraftOk)

    implicitHeight: warningText.height + 8

    radius: 4
    border.width: 2
    border.color: "red"

    state: "ok"

    states: [
        State {
            name: "ok"
        },

        State {
            name: "sim-version-too-low"
            when: aircraftStatus == LocalAircraftCache.AircraftIncompatible

            PropertyChanges {
                target: warningText
                text: "This aircraft has not been declared as compatible with this FlightGear version.  Aircraft may not work as expected."
            }
        },

        State {
            name: "unmaintained"
            when: aircraftStatus == LocalAircraftCache.AircraftUnmaintained

            PropertyChanges {
                target: warningText
                text: "This aircraft is in need of maintenance, and may have bugs or incompatibilities. "
                      + "If you would like to contribute to fixing and improving this aircraft, please see "
                      + "<a href=\"\">this page on our wiki</a>.";
            }
        }

    ]

    Text {
        id: warningText
        anchors.verticalCenter: parent.verticalCenter
        font.bold: true
        linkColor: "#68A6E1"
    }
}
