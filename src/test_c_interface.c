#include <stdio.h>
#include "ros_gst_record_interface/ros_gst_interface.h"

int main(int argc, char *argv[]) {
  VideoRecorder_t recorder;
  Format_t format;
  format.width = 640;
  format.height = 480;
  format.frame_rate = 30;
  VideoRecorder_Init(&recorder, argc, argv);
  VideoRecorder_SetInputFormat(&recorder, &format);
  VideoRecorder_InitPipeline(&recorder);
  VideoRecorder_Close(&recorder); 
  return 0;
}