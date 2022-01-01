cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.90.0") # handled by release scripts
project(KIdleTime VERSION ${KF_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 5.90.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})


include(CMakeFindFrameworks)
include(CMakePackageConfigHelpers)
include(ECMGenerateExportHeader)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMAddQch)

ecm_setup_version(PROJECT VARIABLE_PREFIX KIDLETIME
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kidletime_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5IdleTimeConfigVersion.cmake"
                        SOVERSION 5)

# Dependencies
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)

set(REQUIRED_QT_VERSION 5.15.2)
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui)

if(APPLE)
    cmake_find_frameworks(CoreFoundation Carbon)
endif()

if(NOT APPLE)
    find_package(X11)
    find_package(XCB COMPONENTS XCB)
endif()

if(X11_FOUND)
    if (QT_MAJOR_VERSION STREQUAL "5")
        find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED X11Extras)
    else()
        # qtx11extras_p.h is in Qt6Gui, which implied by the Qt6Gui above
    endif()
    find_package(XCB COMPONENTS XCB SYNC)
    find_package(X11_XCB)
    set(HAVE_X11 TRUE)
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

    if(HAVE_XSCREENSAVER AND NOT WIN32)
        find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED DBus)
    endif()

    if(NOT HAVE_XSCREENSAVER AND NOT HAVE_XSYNC)
        message(FATAL_ERROR "\nNeither the XSync (libXext) nor XScreensaver (libXss) development package was found.\nPlease install one of them (XSync is recommended)")
    endif()
else()
    set(HAVE_X11 FALSE)
endif()

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f02)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055900)
# Subdirectories
add_subdirectory(src)
add_subdirectory(examples)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5IdleTime")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5IdleTime_QCH
        FILE KF5IdleTimeQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5IdleTimeQchTargets.cmake\")")
endif()

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5IdleTimeConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5IdleTimeConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5IdleTimeConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5IdleTimeConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5IdleTimeTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5IdleTimeTargets.cmake NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kidletime_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF} COMPONENT Devel)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
