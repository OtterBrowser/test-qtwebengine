cmake_minimum_required(VERSION 3.1.0)

project(test-qtwebengine)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5 5.6.0 REQUIRED COMPONENTS Core Gui WebEngineWidgets Widgets)

set(testqtwebengine_src
	main.cpp
	MainWindow.cpp
)

qt5_wrap_ui(testqtwebengine_ui
	MainWindow.ui
)

add_executable(test-qtwebengine
	${testqtwebengine_src}
	${testqtwebengine_ui}
)

target_link_libraries(test-qtwebengine Qt5::Core Qt5::Gui Qt5::WebEngineCore Qt5::WebEngineWidgets Qt5::Widgets)

install(TARGETS test-qtwebengine DESTINATION bin/)
