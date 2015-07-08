######## kidletime ####################

configure_file(config-kidletime.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kidletime.h )

set(kidletime_LIB_SRCS
    logging.cpp
    kidletime.cpp
    abstractsystempoller.cpp
    widgetbasedpoller.cpp # TODO: make optional, to avoid always depending on QtWidgets? Or port to QtGui?
)

set(kidletime_LIBS)

add_library(KF5IdleTime ${kidletime_LIB_SRCS})
generate_export_header(KF5IdleTime BASE_NAME KIdleTime)
add_library(KF5::IdleTime ALIAS KF5IdleTime)
target_include_directories(KF5IdleTime INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KIdleTime>")

target_link_libraries(KF5IdleTime PUBLIC Qt5::Core)
target_link_libraries(KF5IdleTime PRIVATE Qt5::Widgets ${kidletime_LIBS})

set_target_properties(KF5IdleTime PROPERTIES  VERSION ${KIDLETIME_VERSION_STRING}
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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KIdleTime COMPONENT Devel
)

install(
    FILES
        abstractsystempoller.h
    DESTINATION
        ${KDE_INSTALL_INCLUDEDIR_KF5}/KIdleTime/private
    COMPONENT
        Devel
)

install(TARGETS KF5IdleTime EXPORT KF5IdleTimeTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KIdleTime LIB_NAME KF5IdleTime DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KIdleTime)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
