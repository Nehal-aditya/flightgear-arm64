# SPDX-FileCopyrightText: 2025 Florent Rougon
# SPDX-License-Identifier: GPL-2.0-or-later

set(TESTSUITE_SOURCES
    ${TESTSUITE_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/TestSuite.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/test_FGTranslate.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/test_TranslationResource.cxx
    PARENT_SCOPE
)

set(TESTSUITE_HEADERS
    ${TESTSUITE_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_FGTranslate.hxx
    ${CMAKE_CURRENT_SOURCE_DIR}/test_TranslationResource.hxx
    PARENT_SCOPE
)
