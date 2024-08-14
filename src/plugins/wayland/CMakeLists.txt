add_library(KF6IdleTimeWaylandPlugin MODULE)

if (Qt6_VERSION VERSION_GREATER_EQUAL "6.8.0")
        set(private_code_option  "PRIVATE_CODE")
endif()
qt6_generate_wayland_protocol_client_sources(KF6IdleTimeWaylandPlugin FILES
    ${PLASMA_WAYLAND_PROTOCOLS_DIR}/idle.xml
    ${WaylandProtocols_DATADIR}/staging/ext-idle-notify/ext-idle-notify-v1.xml
    ${private_code_option}
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
