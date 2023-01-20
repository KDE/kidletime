add_library(KF6IdleTimeWindowsPlugin MODULE)

target_sources(KF6IdleTimeWindowsPlugin PRIVATE
    windowspoller.cpp
)

target_link_libraries(KF6IdleTimeWindowsPlugin
    KF6IdleTime
)

install(
    TARGETS
        KF6IdleTimeWindowsPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/org.kde.kidletime.platforms/
)
