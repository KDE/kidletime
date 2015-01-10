######## kidletime ####################

if (HAVE_XSYNC)
   include_directories( ${XCB_XCB_INCLUDE_DIR} ${XCB_SYNC_INCLUDE_DIR})
endif ()

configure_file(config-kidletime.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kidletime.h )

set(kidletime_LIB_SRCS
    kidletime.cpp
    abstractsystempoller.cpp
    widgetbasedpoller.cpp # TODO: make optional, to avoid always depending on QtWidgets? Or port to QtGui?
)

set(kidletime_LIBS)

if (X11_FOUND)
   if (HAVE_XSCREENSAVER)
      set(kidletime_LIB_SRCS ${kidletime_LIB_SRCS} xscreensaverbasedpoller.cpp)
      qt5_add_dbus_interface(kidletime_LIB_SRCS org.freedesktop.ScreenSaver.xml screensaver_interface)
      set(kidletime_LIBS ${kidletime_LIBS} ${X11_Xscreensaver_LIB} Qt5::DBus)
   endif ()
   if (HAVE_XSYNC)
      set(kidletime_LIB_SRCS ${kidletime_LIB_SRCS} xsyncbasedpoller.cpp )
      set(kidletime_LIBS ${kidletime_LIBS} ${X11_Xext_LIB} ${XCB_XCB_LIBRARY} ${X11_XCB_LIBRARIES} ${XCB_SYNC_LIBRARY})
   endif ()
   set(kidletime_LIBS ${kidletime_LIBS} ${X11_LIBRARIES} Qt5::X11Extras)
elseif (APPLE)
   set(kidletime_LIB_SRCS ${kidletime_LIB_SRCS} macpoller.cpp)
   set(kidletime_LIBS "-framework CoreFoundation -framework Carbon")
elseif (WIN32)
   set(kidletime_LIB_SRCS ${kidletime_LIB_SRCS} windowspoller.cpp)
endif ()

add_library(KF5IdleTime ${kidletime_LIB_SRCS})
generate_export_header(KF5IdleTime BASE_NAME KIdleTime)
add_library(KF5::IdleTime ALIAS KF5IdleTime)
target_include_directories(KF5IdleTime INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KIdleTime>")

target_link_libraries(KF5IdleTime PUBLIC Qt5::Core)
target_link_libraries(KF5IdleTime PRIVATE Qt5::Widgets ${kidletime_LIBS})

set_target_properties(KF5IdleTime PROPERTIES  VERSION ${KIDLETIME_VERSION_STRING}
                                              SOVERSION ${KIDLETIME_SOVERSION}
                                              EXPORT_NAME IdleTime
)

ecm_generate_headers(KIdleTime_HEADERS
  HEADER_NAMES
  KIdleTime

  REQUIRED_HEADERS KIdleTime_HEADERS
)


install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kidletime_export.h
  ${KIdleTime_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KIdleTime COMPONENT Devel
)

install(TARGETS KF5IdleTime EXPORT KF5IdleTimeTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KIdleTime LIB_NAME KF5IdleTime DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KIdleTime)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
