cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

  # AUTOMOC/AUTOUIC on generated files policy.
  if(POLICY CMP0071)
      cmake_policy(SET CMP0071 NEW)
  endif()



project(FGQCanvas)

include(GNUInstallDirs)

option(CHECK_FOR_QT5        "Set to ON to attempt to look for Qt5" ON)
option(CHECK_FOR_QT6        "Set to ON to attempt to look for Qt6" ON)

set(qtNames "")
  if (CHECK_FOR_QT5)
      message(STATUS "Will check for Qt5 libraries")
      list(APPEND qtNames "Qt5")
  endif()
  if (CHECK_FOR_QT6)
      list(APPEND qtNames "Qt6")
  endif()


find_package(QT NAMES ${qtNames} COMPONENTS Core)

if (NOT QT_FOUND)
  message(ERROR "FGQCanvas requested, but Qt not detected" )
endif()

if (${QT_VERSION_MAJOR} EQUAL "5")
    # can't raise this until we update our AppImage build tooling
    set(minQtVersion 5.12.1)
endif()

find_package(Qt${QT_VERSION_MAJOR} ${minQtVersion} REQUIRED COMPONENTS Gui Quick WebSockets)
if (NOT Qt${QT_VERSION_MAJOR}WebSockets_FOUND)
    message(FATAL_ERROR "FGQCanvas enabled, Qt was found but some required packages are missing, stopping.")
endif()

set(SOURCES
  main.cpp
  localprop.cpp
  localprop.h
  fgcanvaselement.cpp
  fgcanvaselement.h
  fgcanvasgroup.cpp
  fgcanvasgroup.h
  fgcanvaspaintcontext.cpp
  fgcanvaspaintcontext.h
  fgcanvaspath.cpp
  fgcanvaspath.h
  fgcanvastext.cpp
  fgcanvastext.h
  fgqcanvasimage.cpp
  fgqcanvasimage.h
  fgqcanvasmap.cpp
  fgqcanvasmap.h
  canvastreemodel.cpp
  canvastreemodel.h
  fgqcanvasfontcache.cpp
  fgqcanvasfontcache.h
  fgqcanvasimageloader.cpp
  fgqcanvasimageloader.h
  elementdatamodel.cpp
  elementdatamodel.h
  canvasitem.cpp
  canvasitem.h
  canvasconnection.cpp
  canvasconnection.h
  applicationcontroller.cpp
  applicationcontroller.h
  canvasdisplay.cpp
  canvasdisplay.h
  canvaspainteddisplay.cpp
  canvaspainteddisplay.h
  jsonutils.cpp
  jsonutils.h
  WindowData.cpp
  WindowData.h
)

qt_add_resources(qrc_sources fgqcanvas_resources.qrc)

add_executable(fgqcanvas ${SOURCES} ${qrc_sources})

set_property(TARGET fgqcanvas PROPERTY AUTOMOC ON)
target_link_libraries(fgqcanvas Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::WebSockets Qt${QT_VERSION_MAJOR}::Quick)

target_include_directories(fgqcanvas PRIVATE ${PROJECT_SOURCE_DIR})

# so ui_foo.h files are found
target_include_directories(fgqcanvas PRIVATE ${PROJECT_BINARY_DIR})

# target_include_directories(fgqcanvas PRIVATE ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
# target_include_directories(fgqcanvas PRIVATE ${Qt5Quick_PRIVATE_INCLUDE_DIRS})

install(TARGETS fgqcanvas RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#### packaging 
if (NOT CPack_CMake_INCLUDED)

  set(CPACK_PACKAGE_NAME ${PROJECT_NAME}
      CACHE STRING "The resulting package name"
  )

  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Light-weight remote client for FlightGear Canvases, implementing using Qt.")
  set(CPACK_PACKAGE_VENDOR "flightgear.org")
  set(CPACK_VERBATIM_VARIABLES YES)

  set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
  SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")

  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

  set(CPACK_PACKAGE_CONTACT "james@flightgear.org")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "James Turner")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "qml6-module-qtqml-workerscript, qml6-module-qtquick-controls, qml6-module-qtquick-templates")

  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE.md")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

  # package name for deb. If set, then instead of some-application-0.9.2-Linux.deb
  # you'll get some-application_0.9.2_amd64.deb (note the underscores too)
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)


  include (CPack)
endif()

####