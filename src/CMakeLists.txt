configure_file(config-kidletime.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kidletime.h )

add_library(KF6IdleTime)
add_library(KF6::IdleTime ALIAS KF6IdleTime)

set_target_properties(KF6IdleTime PROPERTIES
    VERSION     ${KIDLETIME_VERSION}
    SOVERSION   ${KIDLETIME_SOVERSION}
    EXPORT_NAME IdleTime
)

target_sources(KF6IdleTime PRIVATE
    kidletime.cpp
    kabstractidletimepoller.cpp
    kwindowbasedidletimepoller.cpp
)

ecm_qt_declare_logging_category(KF6IdleTime
    HEADER logging.h
    IDENTIFIER KIDLETIME
    CATEGORY_NAME kf.idletime
    OLD_CATEGORY_NAMES org.kde.kf5.idletime
    DESCRIPTION "kidletime (lib)"
    EXPORT KIDLETIME
)

ecm_generate_export_header(KF6IdleTime
    BASE_NAME KIdleTime
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    USE_VERSION_HEADER
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KF6IdleTime INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KIdleTime>")

target_link_libraries(KF6IdleTime PUBLIC Qt6::Core)
target_link_libraries(KF6IdleTime PRIVATE Qt6::Gui)

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
        kabstractidletimepoller_p.h
    DESTINATION
        ${KDE_INSTALL_INCLUDEDIR_KF}/KIdleTime/private
    COMPONENT
        Devel
)

install(TARGETS KF6IdleTime EXPORT KF6IdleTimeTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})


ecm_qt_install_logging_categories(
    EXPORT KIDLETIME
    FILE kidletime.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

ecm_generate_qdoc(KF6IdleTime kidletime.qdocconf)
