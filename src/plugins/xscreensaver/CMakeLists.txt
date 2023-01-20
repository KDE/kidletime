add_library(KF6IdleTimeXcbPlugin1 MODULE)

set(xscreensaver_plugin_dbus_SRCS)
qt_add_dbus_interface(xscreensaver_plugin_dbus_SRCS org.freedesktop.ScreenSaver.xml screensaver_interface)

target_sources(KF6IdleTimeXcbPlugin1 PRIVATE
    xscreensaverbasedpoller.cpp
    ${xscreensaver_plugin_dbus_SRCS}
)

target_link_libraries(KF6IdleTimeXcbPlugin1
    KF6IdleTime
    Qt${QT_MAJOR_VERSION}::DBus
    ${X11_LIBRARIES}
    ${X11_Xscreensaver_LIB}
)

if (TARGET Qt5::X11Extras)
    target_link_libraries(KF6IdleTimeXcbPlugin1 Qt5::X11Extras)
elseif (TARGET Qt6::Gui)
    target_link_libraries(KF6IdleTimeXcbPlugin1 Qt6::Gui)
endif()


install(
    TARGETS
        KF6IdleTimeXcbPlugin1
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/org.kde.kidletime.platforms/
)
