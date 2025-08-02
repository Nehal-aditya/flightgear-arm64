# SPDX-FileCopyrightText: 2018 Edware d'Auvergne
# SPDX-License-Identifier: GPL-2.0-or-later

set(TESTSUITE_SOURCES
    ${TESTSUITE_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/TestSuite.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/testAeroElement.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/testFGInterface.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/testYASimAtmosphere.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/testYASimGear.cxx
    PARENT_SCOPE
)

set(TESTSUITE_HEADERS
    ${TESTSUITE_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/testAeroElement.hxx
    ${CMAKE_CURRENT_SOURCE_DIR}/testFGInterface.hxx
    ${CMAKE_CURRENT_SOURCE_DIR}/testYASimAtmosphere.hxx
    ${CMAKE_CURRENT_SOURCE_DIR}/testYASimGear.hxx
    PARENT_SCOPE
)
