
// SPDX-FileCopyrightText: 2017 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.4
import FlightGear.Launcher 1.0
import FlightGear 1.0

Item {
    id: root

    property url uri
    property int installStatus: LocalAircraftCache.PackageNotInstalled
    property int packageSize: 0

    property int downloadedBytes: 0

    readonly property bool active: (installStatus == LocalAircraftCache.PackageQueued) ||
                                   (installStatus == LocalAircraftCache.PackageDownloading)

    readonly property int compactWidth: button.width + sizeText.width

    property bool compact: false

    implicitWidth: childrenRect.width
    implicitHeight: Math.max(button.height, sizeText.height, progressColumn.height)

    state: "not-installed"

    function updateState()
    {
        if (!_launcher.isNetworkAvailable) {
            if (installStatus == LocalAircraftCache.PackageInstalled) {
                state = "installed";
            } else {
                state = "offline";
            }
            return;
        }

        if (installStatus == LocalAircraftCache.PackageInstalled) {
            state = "installed";
        } else if (installStatus == LocalAircraftCache.PackageNotInstalled) {
            state = "not-installed"
        } else if (installStatus == LocalAircraftCache.PackageUpdateAvailable) {
            state = "has-update"
        } else if (installStatus == LocalAircraftCache.PackageQueued) {
            state = "queued"
        } else if (installStatus == LocalAircraftCache.PackageDownloading) {
            state = "downloading"
        }
    }

    onInstallStatusChanged: updateState()

    states: [
        State {
            name: "not-installed"

            PropertyChanges {
                target: button
                text: qsTr("Install")
                hoverText: ""
                visible: true
            }

            PropertyChanges { target: sizeText; visible: true }
            PropertyChanges { target: confirmUninstallPanel; visible: false }
            PropertyChanges { target: uninstallButton;  visible: false }
        },

        State {
            name: "installed"

            PropertyChanges { target: uninstallButton;  visible: true }
            PropertyChanges { target: sizeText; visible: true }
            PropertyChanges { target: confirmUninstallPanel; visible: false }
            PropertyChanges { target: button; visible: false }
        },

        State {
            name: "has-update"

            PropertyChanges {
                target: button
                text: qsTr("Update")
                hoverText: ""
                visible: true
            }

            PropertyChanges { target: uninstallButton;  visible: true }
            PropertyChanges { target: sizeText; visible: true }
            PropertyChanges { target: confirmUninstallPanel; visible: false }
        },

        State {
            name: "queued"

            PropertyChanges {
                target: sizeText
                visible: true
            }

            PropertyChanges {
                target: button
                text: qsTr("Queued")
                hoverText: qsTr("Cancel")
                visible: true
            }

            PropertyChanges { target: uninstallButton;  visible: false }
            PropertyChanges { target: confirmUninstallPanel; visible: false }
        },

        State {
            name: "downloading"
            PropertyChanges { target: progressFrame; visible: true }
            PropertyChanges { target: statusText; visible: true }
            PropertyChanges { target: sizeText; visible: false }
            PropertyChanges { target: confirmUninstallPanel; visible: false }
            PropertyChanges { target: uninstallButton;  visible: false }

            PropertyChanges {
                target: button
                text: qsTr("Downloading")
                hoverText: qsTr("Cancel")
                visible: true
            }
        },

        State {
            name: "confirm-uninstall"
            PropertyChanges { target: button; visible: false }
            PropertyChanges { target: uninstallButton;  visible: false }
            PropertyChanges { target: progressFrame; visible: false }
            PropertyChanges { target: statusText; visible: false }
            PropertyChanges { target: sizeText; visible: false }
            PropertyChanges { target: confirmUninstallPanel; visible: true }
        },

        State {
            name: "offline"

            PropertyChanges { target: uninstallButton;  visible: false }
            PropertyChanges { target: sizeText; visible: true }
            PropertyChanges { target: confirmUninstallPanel; visible: false }
            PropertyChanges { target: button; visible: false }
        }
    ]

    Row {
        id: buttonRow
        spacing: Style.margin
        Button {
            id: button
            onClicked: {
                if ((root.state == "has-update") || (root.state == "not-installed")) {
                    _launcher.requestInstallUpdate(root.uri);
                } else {
                    _launcher.requestInstallCancel(root.uri)
                }
            }
        }

        Button {
            id: uninstallButton
            text: qsTr("Uninstall")
            anchors.verticalCenter: button.verticalCenter
            onClicked: root.state = "confirm-uninstall"
        }
    }

    StyledText {
        id: sizeText
        anchors.left: buttonRow.right
        anchors.leftMargin: Style.margin
        anchors.right: parent.right
        anchors.verticalCenter: buttonRow.verticalCenter
        text: qsTr("Size: ") + (root.packageSize / 0x100000).toFixed(1) + qsTr(" MB")
    }

    Column {
        id: progressColumn

        anchors.verticalCenter: buttonRow.verticalCenter
        anchors.left: buttonRow.right
        anchors.leftMargin: Style.margin
        anchors.right: parent.right

        Rectangle {
            id: progressFrame
            radius: 6
            height: 12

            width: parent.width - (Style.margin * 2)
            visible: false // hidden by default

            border.color: Style.minorFrameColor
            border.width: 2

            Rectangle {
                id: progressBar

                radius:3
                height: 6
                anchors.verticalCenter: parent.verticalCenter

                color: Style.themeColor

                readonly property real fraction: root.downloadedBytes / root.packageSize
                readonly property real maxWidth: parent.width - 10

                x: 5
                width: maxWidth * fraction
            }
        }

        // show download progress textually, or error message if a problem occurs
        StyledText {
            id: statusText
            visible: false
            text: (compact ? "" : qsTr("Downloaded ")) + (root.downloadedBytes / 0x100000).toFixed(1) +
                qsTr(" MB of ") + (root.packageSize / 0x100000).toFixed(1) + qsTr(" MB");
        }
    } // item container for progress bar and text



    YesNoPanel {
        id: confirmUninstallPanel
        anchors.fill: parent
        promptText: qsTr("Are you sure you want to uninstall this aircraft?")
        yesIsDestructive: true
        yesText: qsTr("Uninstall")
        noText: qsTr("Cancel")

        // revert to the current underlying state
        onRejected: updateState()
        onAccepted: _launcher.requestUninstall(root.uri)
     }
}
