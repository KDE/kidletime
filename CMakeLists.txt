cmake_minimum_required(VERSION 2.8.12)

project(KIdleTime)

# ECM setup
find_package(ECM 1.5.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(CMakeFindFrameworks)
include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

set(KF5_VERSION "5.5.0") # handled by release scripts

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX KIDLETIME
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kidletime_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5IdleTimeConfigVersion.cmake"
                        SOVERSION 5)

# Dependencies
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.2.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets)

if(NOT WIN32 AND NOT APPLE)
    find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED X11Extras DBus)
endif()

if(APPLE)
    cmake_find_frameworks(CoreFoundation Carbon)
endif()

if(NOT APPLE)
    find_package(X11)
    find_package(XCB COMPONENTS XCB)
endif()

if(X11_FOUND)
    find_package(XCB COMPONENTS XCB SYNC)
    find_package(X11_XCB)
    if (X11_XSync_FOUND AND XCB_XCB_FOUND AND X11_XCB_FOUND AND XCB_SYNC_FOUND)
        # One cannot just use "set(HAVE_XSYNC X11_XSync_FOUND AND XCB_XCB_FOUND...)"
        # Doing so would create a list because the value passed to set() is not evaluated.
        set(HAVE_XSYNC TRUE)
    else()
        set(HAVE_XSYNC FALSE)
    endif()
    add_feature_info("libXext" X11_XSync_FOUND "required for X-Sync based poller")
    add_feature_info("libX11-xcb" X11_XCB_FOUND "required for X-Sync based poller")
    add_feature_info("libxcb" XCB_XCB_FOUND "required for X-Sync based poller")
    add_feature_info("libxcb-sync" XCB_SYNC_FOUND "required for X-Sync based poller")
    add_feature_info("X-Sync based poller" HAVE_XSYNC "efficient operation of KIdleTime. STRONGLY RECOMMENDED (requires libXext, libX11-xcb, libxcb and libxcb-sync)")

    set(HAVE_XSCREENSAVER ${X11_Xscreensaver_FOUND})
    add_feature_info("X-Screensaver based poller" HAVE_XSCREENSAVER "KIdleTime fallback mode (libXss)")

    if(NOT HAVE_XSCREENSAVER AND NOT HAVE_XSYNC)
        message(FATAL_ERROR "\nNeither the XSync (libXext) nor XScreensaver (libXss) development package was found.\nPlease install one of them (XSync is recommended)")
    endif()
endif()

# Subdirectories
add_subdirectory(src)
add_subdirectory(examples)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5IdleTime")

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5IdleTimeConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5IdleTimeConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5IdleTimeConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5IdleTimeConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5IdleTimeTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5IdleTimeTargets.cmake NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kidletime_version.h
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR} COMPONENT Devel)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
