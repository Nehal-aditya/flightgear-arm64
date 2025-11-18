# SPDX-FileCopyrightText: James Turner <james@flightgear.org>
# SPDX-License-Identifier: GPL-2.0-or-later

# read 'version' file into a variable (stripping any newlines or spaces)
file(READ flightgear-version versionFile)
if (NOT versionFile)
    message(FATAL_ERROR "Unable to determine FlightGear version. Version file is missing.")
endif()
string(STRIP "${versionFile}" FLIGHTGEAR_VERSION)
# add a dependency on the version file
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS version)

include(GetGitRevisionDescription)

find_package(Git)

git_describe(GIT_DESCRIBE --always)

# Convert to SemVer format
# https://semver.org/,
set(SEMVER_REGEX_PATTERN "^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)-?([a-zA-Z][0-9a-zA-Z\.]*)?-?(.*)?$")
string(REGEX MATCH ${SEMVER_REGEX_PATTERN} MATCHED_GIT_DESC ${GIT_DESCRIBE})

if (CMAKE_MATCH_4)
    message(STATUS "Have Git pre-release label in tag")
    set(INSTALLER_RELEASE_SUFFIX "-${CMAKE_MATCH_4}")
else()
    message(STATUS "No pre-release version set")
endif()

if (FG_BUILD_TYPE STREQUAL "Nightly")
    string(TIMESTAMP BUILD_DATE "%Y%m%d")
elseif(FG_BUILD_TYPE STREQUAL "Dev")
    # we don't use GIT_REF
    get_git_head_revision(GIT_REF GIT_FULL_SHA)

    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short=8 ${GIT_FULL_SHA}
        OUTPUT_VARIABLE GIT_SHA
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()


# FlightGear and SimGear versions need to match major + minor
# split version string into components, note CMAKE_MATCH_0 is the entire regexp match
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" VERSION_REGEX ${FLIGHTGEAR_VERSION} )
set(FG_VERSION_MAJOR ${CMAKE_MATCH_1})
set(FG_VERSION_MINOR ${CMAKE_MATCH_2})
set(FG_VERSION_PATCH ${CMAKE_MATCH_3})
