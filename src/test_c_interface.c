#include <stdio.h>
#include <stdlib.h>
#include "ros_gst_record_interface/ros_gst_interface.h"

int main(int argc, char *argv[]) {
  VideoRecorder_t *recorder = (VideoRecorder_t*)malloc(sizeof(VideoRecorder_t));
  Format_t format;
  format.width = 640;
  format.height = 480;
  format.frame_rate = 30;
  VideoRecorder_Init(recorder, argc, argv);
  VideoRecorder_SetInputFormat(recorder, &format);
  VideoRecorder_SetOutputFormat(recorder, &format);
  VideoRecorder_InitPipeline(recorder);
  VideoRecorder_StartStream(recorder);
  VideoRecorder_Close(recorder);
  return 0;
}
