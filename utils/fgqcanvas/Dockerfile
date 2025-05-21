# SPDX-FileCopyrightText: James Turner <james@flightgear.org>
#
# SPDX-License-Identifier: GPL-2.0

# rebuild commands 
#   docker build -t registry.gitlab.com/flightgear/flightgear/fgqcanvas-raspbian flightgear/utils/fgqcanvas
#   docker push registry.gitlab.com/flightgear/flightgear/fgqcanvas-raspbian

FROM --platform=linux/arm64 dtcooper/raspberrypi-os:bookworm

RUN export DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
    && apt-get install --yes \
        build-essential \
        cmake \
        software-properties-common \
        file \
        ninja-build \
        libssl-dev \
        libfuse2 \
        fuse \
        libxkbcommon-dev \
        qt6-base-dev \
        qt6-declarative-dev \
        qt6-tools-dev \
        qt6-tools-dev-tools \
        libqt6websockets6-dev \
        qt6-l10n-tools \ 
        libqt6svg6-dev \
        qml6-module-qtquick-controls \
        qml6-module-qtquick \
    	qml6-module-qtquick-templates \
        qml6-module-qtqml-workerscript \
        qmake6 \
        wget \
        ca-certificates gpg \
        git
