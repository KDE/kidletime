include_directories( ${XCB_XCB_INCLUDE_DIR} ${XCB_SYNC_INCLUDE_DIR})

set(xsync_plugin_SRCS
    xsyncbasedpoller.cpp
    ../../logging.cpp
)

ecm_qt_declare_logging_category(xsync_plugin_SRCS HEADER xsync_logging.h IDENTIFIER KIDLETIME_XSYNC_PLUGIN CATEGORY_NAME org.kde.kf5.idletime.xsync)

add_library(KF5IdleTimeXcbPlugin0 MODULE ${xsync_plugin_SRCS})
target_link_libraries(KF5IdleTimeXcbPlugin0
    KF5IdleTime
    Qt5::X11Extras
    XCB::XCB
    XCB::SYNC
    X11::XCB
    ${X11_LIBRARIES}
    ${X11_Xext_LIB}
)

install(
    TARGETS
        KF5IdleTimeXcbPlugin0
    DESTINATION
        ${PLUGIN_INSTALL_DIR}/kf5/org.kde.kidletime.platforms/
)
