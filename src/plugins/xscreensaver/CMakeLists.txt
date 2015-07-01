set(xscreensaver_plugin_SRCS
    xscreensaverbasedpoller.cpp
)

qt5_add_dbus_interface(xscreensaver_plugin_SRCS org.freedesktop.ScreenSaver.xml screensaver_interface)

add_library(KF5IdleTimeXcbPlugin1 MODULE ${xscreensaver_plugin_SRCS})
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
        ${PLUGIN_INSTALL_DIR}/kf5/org.kde.kidletime.platforms/
)

