#include <stdio.h>
#include "ros_gst_record_interface/ros_gst_interface.h"

int main(int argc, char *argv[]) {
  VideoRecorder_t recorder;
  
  VideoRecorder_Init(&recorder, argc, argv);
  VideoRecorder_Close(&recorder); 
  return 0;
}