configure_file(config-kidletime.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kidletime.h )

add_library(KF5IdleTime)
add_library(KF5::IdleTime ALIAS KF5IdleTime)

target_sources(KF5IdleTime PRIVATE
    kidletime.cpp
    abstractsystempoller.cpp
    widgetbasedpoller.cpp # TODO it doesn't actually use widgets, just GUI. Rename?
)

ecm_qt_declare_logging_category(KF5IdleTime
    HEADER logging.h
    IDENTIFIER KIDLETIME
    CATEGORY_NAME kf.idletime
    OLD_CATEGORY_NAMES org.kde.kf5.idletime
    DESCRIPTION "kidletime (lib)"
    EXPORT KIDLETIME
)

ecm_generate_export_header(KF5IdleTime
    BASE_NAME KIdleTime
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.76
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)
target_include_directories(KF5IdleTime INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KIdleTime>")

target_link_libraries(KF5IdleTime PUBLIC Qt${QT_MAJOR_VERSION}::Core)
target_link_libraries(KF5IdleTime PRIVATE Qt${QT_MAJOR_VERSION}::Gui)

set_target_properties(KF5IdleTime PROPERTIES  VERSION ${KIDLETIME_VERSION}
                                              SOVERSION ${KIDLETIME_SOVERSION}
                                              EXPORT_NAME IdleTime
)

add_subdirectory(plugins)

ecm_generate_headers(KIdleTime_HEADERS
  HEADER_NAMES
  KIdleTime

  REQUIRED_HEADERS KIdleTime_HEADERS
)


install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kidletime_export.h
  ${KIdleTime_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KIdleTime COMPONENT Devel
)

install(
    FILES
        abstractsystempoller.h
    DESTINATION
        ${KDE_INSTALL_INCLUDEDIR_KF}/KIdleTime/private
    COMPONENT
        Devel
)

install(TARGETS KF5IdleTime EXPORT KF5IdleTimeTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})


ecm_qt_install_logging_categories(
    EXPORT KIDLETIME
    FILE kidletime.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

if(BUILD_QCH)
    ecm_add_qch(
        KF5IdleTime_QCH
        NAME KIdleTime
        BASE_NAME KF5IdleTime
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KIdleTime_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KIDLETIME_EXPORT
            "KIDLETIME_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KIdleTime LIB_NAME KF5IdleTime DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KIdleTime)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
