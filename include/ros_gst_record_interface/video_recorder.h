#ifndef VIDEO_RECORDER_H_
#define VIDEO_RECORDER_H_
#include <stdin.h>
#include "ros_gst_record_interface/ros_gst_interface.h"

namespace gst_ros {
class VideoRecorder {
 public:
  VideoRecorder();
  ~VideoRecorder();

  int32_t InitializeRecording();
  int32_t SetInputVideoFormat();
  int32_t SetOutputVideoFormat();
  int32_t StreamVideo();

  int32_t StartStream();
  int32_t StopStream();
  int32_t SubmitFrame();
 
 private:
  VideoRecorder_t *vr;
};
}  // namespace gst_ros

#endif  // VIDEO_RECORDER_H_