set(osx_plugin_SRCS
    macpoller.cpp
)

add_library(KF5IdleTimeOsxPlugin MODULE ${osx_plugin_SRCS})
target_link_libraries(KF5IdleTimeOsxPlugin
    KF5IdleTime
    "-framework CoreFoundation -framework Carbon"
)

install(
    TARGETS
        KF5IdleTimeOsxPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf5/org.kde.kidletime.platforms/
)
