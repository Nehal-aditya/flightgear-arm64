# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileCopyrightText: 2020 Edward d'Auvergne

# Add each simgear test category.
foreach( simgear_test_category
        math
        props
        canvas
        structure
        timing
    )

    add_subdirectory(${simgear_test_category})

endforeach( simgear_test_category )


set(TESTSUITE_SOURCES
    ${TESTSUITE_SOURCES}
    PARENT_SCOPE
)


set(TESTSUITE_HEADERS
    ${TESTSUITE_HEADERS}
    PARENT_SCOPE
)
