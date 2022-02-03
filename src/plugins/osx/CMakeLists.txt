add_library(KF5IdleTimeOsxPlugin MODULE)

target_sources(KF5IdleTimeOsxPlugin PRIVATE
    macpoller.cpp
)

target_link_libraries(KF5IdleTimeOsxPlugin
    KF5IdleTime
    "-framework CoreFoundation -framework Carbon"
)

install(
    TARGETS
        KF5IdleTimeOsxPlugin
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kidletime/
)
