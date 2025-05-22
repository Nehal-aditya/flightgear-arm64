# SPDX-FileCopyrightText: 2025 Florent Rougon
# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileComment: Dummy add-on main Nasal file, for use in automated tests

var main = func(addon) {
    logprint(LOG_INFO, "Test add-on initialized from path ", addon.basePath);
}
