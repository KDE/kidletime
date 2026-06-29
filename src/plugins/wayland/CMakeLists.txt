add_library(KF6IdleTimeWaylandPlugin MODULE)

qt6_generate_wayland_protocol_client_sources(KF6IdleTimeWaylandPlugin
    PRIVATE_CODE
    FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/idle.xml
        ${WaylandProtocols_DATADIR}/staging/ext-idle-notify/ext-idle-notify-v1.xml
)

target_sources(KF6IdleTimeWaylandPlugin PRIVATE poller.cpp ${idletime_plugin_SRCS})

target_link_libraries(KF6IdleTimeWaylandPlugin
    KF6::IdleTime
    Qt6::WaylandClient
    Wayland::Client
)

install(
    TARGETS
        KF6IdleTimeWaylandPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf6/org.kde.kidletime.platforms/
)

ecm_qt_declare_logging_category(KF6IdleTimeWaylandPlugin
    HEADER wayland_logging.h
    IDENTIFIER KIDLETIME_WAYLAND_PLUGIN
    CATEGORY_NAME kf.idletime.wayland
    OLD_CATEGORY_NAMES kf5idletime_wayland
    DESCRIPTION "KIdleTime Wayland plugin"
    EXPORT KIDLETIME
)
