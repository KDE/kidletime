add_library(KF5IdleTimeXcbPlugin1 MODULE)

set(xscreensaver_plugin_dbus_SRCS)
qt_add_dbus_interface(xscreensaver_plugin_dbus_SRCS org.freedesktop.ScreenSaver.xml screensaver_interface)

target_sources(KF5IdleTimeXcbPlugin1 PRIVATE
    xscreensaverbasedpoller.cpp
    ${xscreensaver_plugin_dbus_SRCS}
)

target_link_libraries(KF5IdleTimeXcbPlugin1
    KF5IdleTime
    Qt5::DBus
    Qt5::X11Extras
    ${X11_LIBRARIES}
    ${X11_Xscreensaver_LIB}
)

install(
    TARGETS
        KF5IdleTimeXcbPlugin1
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf5/org.kde.kidletime.platforms/
)
