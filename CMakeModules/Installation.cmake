# SPDX-FileCopyrightText: James Turner <james@flightgear.org>
# SPDX-License-Identifier: GPL-2.0-or-later

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

if (TARGET sentry_crashpad::handler)
    if (APPLE)
        # install inside the bundle
        install(FILES $<TARGET_FILE:sentry_crashpad::handler> DESTINATION fgfs.app/Contents/MacOS OPTIONAL)
    else()
        # install in the bin-dir, next to the application binary
        install(FILES $<TARGET_FILE:sentry_crashpad::handler> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
    endif()
endif()

if (HAVE_QT)
    include (QtDeployment)
endif()


if (MSVC)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/generateInnoSetupConfig.cmake.in
        ${CMAKE_BINARY_DIR}/generateInnoSetupConfig.cmake
        @ONLY)

    install(SCRIPT ${CMAKE_BINARY_DIR}/generateInnoSetupConfig.cmake COMPONENT packaging )

    # important we use install() here so that passing a custom prefix to
    # 'cmake --install --prefix FOO' works correctly to put the file somewhere special
    install(FILES ${CMAKE_BINARY_DIR}/InstallConfig.iss DESTINATION . COMPONENT packaging )
else()
    configure_file(${CMAKE_CURRENT_LIST_DIR}/exportFGVersion.sh.in
        ${CMAKE_BINARY_DIR}/exportFGVersion.sh
        @ONLY)
endif()


########################################################################################

# OSG libs
foreach (osglib OSG OpenThreads osgUtil osgText osgGA osgSim osgParticle osgTerrain osgViewer osgDB)
    if (APPLE)
        install(FILES
                $<TARGET_FILE:OSG::${osglib}>
            DESTINATION
                $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/Frameworks
        )
    endif()
endforeach()

if (APPLE)
    # OSG plugins
    install(DIRECTORY ${OSG_PLUGINS_DIR} DESTINATION $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/PlugIns)

    # add extra utilities to the bundle
    install(TARGETS fgcom fgjs fgelev DESTINATION $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/MacOS)

    if (TARGET sentry::sentry)
        install(FILES $<TARGET_FILE:sentry::sentry> DESTINATION $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/Frameworks)
    endif()

    if (TARGET sentry_crashpad::handler)
        install(FILES $<TARGET_FILE:sentry_crashpad::handler> DESTINATION $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/MacOS)
    endif()

    if (TARGET DBus::DBus)
        #get_target_property(dbusLib DBus::DBus IMPORTED_LOCATION)
        #message(STATUS "DBus library at: ${dbusLib}")
        #install(FILES ${dbusLib} DESTINATION $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/MacOS)
        install(FILES $<TARGET_FILE:DBus::DBus> DESTINATION $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/Frameworks)
    endif()

    # FIXME: this copies the fully version file name, need to rename to the non-versioned one
    install(FILES
            $<TARGET_FILE:OpenAL::OpenAL>
        DESTINATION
            $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/Frameworks
    )

    install(FILES ${CMAKE_SOURCE_DIR}/package/mac/FlightGear.icns DESTINATION $<TARGET_BUNDLE_CONTENT_DIR:fgfs>/Resources)
endif()

########################################################################################
# AppDir creation for Linux AppImage

if (LINUX)

    install(TARGETS fgcom fgjs fgelev fgfs
        DESTINATION appdir/usr/bin
        COMPONENT packaging EXCLUDE_FROM_ALL)

    install(DIRECTORY ${OSG_PLUGINS_DIR}
        DESTINATION appdir/usr/lib
        COMPONENT packaging EXCLUDE_FROM_ALL)

    install(FILES /etc/ssl/certs/ca-certificates.crt
	DESTINATION appdir/usr/ssl
	RENAME cacert.pem
	OPTIONAL
	COMPONENT packaging EXCLUDE_FROM_ALL)
    install(CODE "
	if(NOT EXISTS /etc/ssl/certs/ca-certificates.crt)
            message(WARNING \"No SSL certificates found, will not be included in AppImage\")
	endif()
    ")
    # TODO: things under share/
endif()


########################################################################################
# actual app installation: this needs to happen late, after the various TARGET_BUNDLE_CONTENT_DIR
# rules are applied

if (APPLE)
    install(TARGETS fgfs BUNDLE DESTINATION .)
else()
    install(TARGETS fgfs RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


#-----------------------------------------------------------------------------
### uninstall target
#-----------------------------------------------------------------------------
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
    "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

if (NOT TARGET uninstall)
    ADD_CUSTOM_TARGET(uninstall
        "${CMAKE_COMMAND}" -P "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake")
endif()
