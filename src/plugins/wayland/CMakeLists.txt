add_library(KF5IdleTimeWaylandPlugin MODULE)
if (QT_MAJOR_VERSION EQUAL "5")
    ecm_add_qtwayland_client_protocol(idletime_plugin_SRCS
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/idle.xml
        BASENAME idle
    )
else()
    qt6_generate_wayland_protocol_client_sources(KF5IdleTimeWaylandPlugin FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/idle.xml)
endif()
target_sources(KF5IdleTimeWaylandPlugin PRIVATE poller.cpp ${idletime_plugin_SRCS})

target_link_libraries(KF5IdleTimeWaylandPlugin
    KF5::IdleTime
    Qt${QT_MAJOR_VERSION}::WaylandClient
    Qt${QT_MAJOR_VERSION}::GuiPrivate
    Wayland::Client
)

install(
    TARGETS
        KF5IdleTimeWaylandPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf5/org.kde.kidletime.platforms/
)
