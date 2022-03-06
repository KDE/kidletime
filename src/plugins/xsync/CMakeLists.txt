add_library(KF5IdleTimeXcbPlugin0 MODULE)

target_include_directories(KF5IdleTimeXcbPlugin0
    PRIVATE
        ${XCB_XCB_INCLUDE_DIR}
        ${XCB_SYNC_INCLUDE_DIR}
)

target_sources(KF5IdleTimeXcbPlugin0 PRIVATE
    xsyncbasedpoller.cpp
    ../../logging.cpp
)

ecm_qt_declare_logging_category(KF5IdleTimeXcbPlugin0
    HEADER xsync_logging.h
    IDENTIFIER KIDLETIME_XSYNC_PLUGIN
    CATEGORY_NAME kf.idletime.xsync
    OLD_CATEGORY_NAMES org.kde.kf5.idletime.xsync
    DESCRIPTION "xsync plugin"
    EXPORT KIDLETIME
)

target_link_libraries(KF5IdleTimeXcbPlugin0
    KF5IdleTime
    XCB::XCB
    XCB::SYNC
    X11::XCB
    ${X11_LIBRARIES}
    ${X11_Xext_LIB}
)

if (TARGET Qt5::X11Extras)
    target_link_libraries(KF5IdleTimeXcbPlugin0 Qt5::X11Extras)
elseif (TARGET Qt6::Gui)
    target_link_libraries(KF5IdleTimeXcbPlugin0 Qt6::Gui)
endif()

install(
    TARGETS
        KF5IdleTimeXcbPlugin0
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/org.kde.kidletime.platforms/
)
