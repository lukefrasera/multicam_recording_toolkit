project(tests)


find_package(GStreamer REQUIRED COMPONENTS
  gstreamer-video
  gstreamer-audio
)
find_package(GLIB REQUIRED COMPONENTS
  gobject
  gmodule
)

find_package(OpenCV REQUIRED)

pkg_check_modules (GLIB2   glib-2.0)

include_directories(
	include
	${GSTREAMER_INCLUDE_DIRS}
  ${GSTREAMER_AUDIO_INCLUDE_DIRS}
  ${GLIB_INCLUDE_DIRS}
)

add_executable(video_play
	src/video_play.c
)

target_link_libraries(video_play
	${GSTREAMER_LIBRARIES}
)

add_executable(pipe_test
  src/piping_test.cc
)

# target_link_libraries(pipe_test
#   pstream
# )

add_executable(appsrc
  src/appsrc_test.c
)
target_link_libraries(appsrc
  ${GSTREAMER_LIBRARIES}
  ${GSTREAMER_AUDIO_LIBRARIES}
  ${GLIB_LIBRARIES}
  ${GLIB_GOBJECT_LIBRARIES}
)

add_executable(pipe_video_test
  src/piping_video_test.cc
)

target_link_libraries(pipe_video_test
  ${OpenCV_LIBS})