project(tests)


find_package(GStreamer REQUIRED COMPONENTS
  gstreamer-video
)

include_directories(
	include
	${GSTREAMER_INCLUDE_DIRS}
)

add_executable(video_play
	src/video_play.c
)

target_link_libraries(video_play
	${GSTREAMER_LIBRARIES}
)
