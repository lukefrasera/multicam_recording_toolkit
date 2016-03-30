#include "ros_gst_record_interface/video_recorder.h"

namespace gst_ros {
VideoRecorder::VideoRecorder(int argc, char *argv[]) {
  vr = (VideoRecorder_t*)malloc(sizeof(VideoRecorder_t));

  VideoRecorder_Init(vr, argc, argv);
  // VideoRecorder_SetInputFormat(vr, )
}
VideoRecorder::~VideoRecorder() {}





}  // namespace gst_ros
