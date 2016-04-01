cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
add_subdirectory(tests)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Adding Executable in main build directory
find_package(GStreamer REQUIRED COMPONENTS
        gstreamer-video
        gstreamer-audio
        gstreamer-app)

find_package(GLIB REQUIRED COMPONENTS
  gobject
  gmodule
)

include_directories(include
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_AUDIO_INCLUDE_DIRS}
        ${GSTREAMER_VIDEO_INCLUDE_DIRS}
        ${GSTREAMER_APP_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS})

add_library(ros_gst_interface
        src/ros_gst_record_interface/ros_gst_interface.c)

target_link_libraries(ros_gst_interface
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_AUDIO_LIBRARIES}
        ${GSTREAMER_VIDEO_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${GLIB_GOBJECT_LIBRARIES}
        ${GSTREAMER_APP_LIBRARIES})

add_executable(interface_test
        src/test_c_interface.c)

target_link_libraries(interface_test
        ros_gst_interface)
