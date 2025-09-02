/*
 * SPDX-FileName: AIConstants.hxx
 * SPDX-FileComment: AIConstants
 * SPDX-FileCopyrightText: Copyright (C) 2024 Keith Paterson - keith.paterson@gmx.de
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

namespace AILeg {
enum Type {
    UNKNOWN,
    STARTUP_PUSHBACK,
    RUNWAY_TAXI,
    ALIGN_RUNWAY,
    TAKEOFF,
    CLIMB,
    CRUISE,
    APPROACH,
    HOLD_PATTERN,
    LANDING,
    PARKING_TAXI,
    PARKING
};
}
