add_library(KF5IdleTimeWindowsPlugin MODULE)

target_sources(KF5IdleTimeWindowsPlugin PRIVATE
    windowspoller.cpp
)

target_link_libraries(KF5IdleTimeWindowsPlugin
    KF5IdleTime
)

install(
    TARGETS
        KF5IdleTimeWindowsPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/org.kde.kidletime.platforms/
)
