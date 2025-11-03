# SPDX-FileCopyrightText: (C) 2018 Edward d'Auvergne
# SPDX-License-Identifier: GPL-2.0-or-later


set(TESTSUITE_SOURCES
    ${TESTSUITE_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/TestSuite.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/test_airport.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/test_groundnet.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/test_runway.cxx
    PARENT_SCOPE
)

set(TESTSUITE_HEADERS
    ${TESTSUITE_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_airport.hxx
    ${CMAKE_CURRENT_SOURCE_DIR}/test_groundnet.hxx
    ${CMAKE_CURRENT_SOURCE_DIR}/test_runway.hxx
    PARENT_SCOPE
)
