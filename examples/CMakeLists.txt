add_executable(kidletime_example main.cpp KIdleTest.cpp)
target_link_libraries(kidletime_example KF5::IdleTime Qt${QT_MAJOR_VERSION}::Gui)
