# SPDX-FileCopyrightText: James Turner <james@flightgear.org>
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(Git)
if (Git_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} --git-dir ${SRC}/.git rev-parse  HEAD
        OUTPUT_VARIABLE REVISION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Git revision is ${REVISION}")
else()
    set(REVISION "none")
endif()

string(TIMESTAMP CURRENT_DATE "%Y-%m-%d")
string(TIMESTAMP CURRENT_YEAR_MONTH "%Y-%m")

configure_file (${SRC}/src/Include/flightgearBuildId.h.cmake-in ${DST})
