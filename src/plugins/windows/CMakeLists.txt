set(windows_plugin_SRCS
    windowspoller.cpp
)

add_library(KF5IdleTimeWindowsPlugin MODULE ${windows_plugin_SRCS})
target_link_libraries(KF5IdleTimeWindowsPlugin
    KF5IdleTime
)

install(
    TARGETS
        KF5IdleTimeWindowsPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf5/org.kde.kidletime.platforms/
)
