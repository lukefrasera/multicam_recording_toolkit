#include "ros_gst_record_interface/ros_gst_interface.h"

void VideoRecorder_StartFeed(GstElement *source, VideoRecorder_t *vr) {

}

void VideoRecorder_StopFeed(GstElement *source, VideoRecorder_t *vr) {

}

void VideoRecorder_SampleFeed(GstElement *sink, VideoRecorder_t *vr) {
  GstSample *sample;

  g_signal_emit_by_name(sink, "pull-sample", &sample);
  if (sample) {
    g_print("*");
    gst_sample_unref(sample);
  }
}

int32_t VideoRecorder_Init(VideoRecorder_t *vr, int argc, char *argv[]) {
  gst_init(&argc, &argv);
  
  vr->app_source = gst_element_factory_make("appsrc", "video_source");
  vr->video_queue = gst_element_factory_make("queue", "video_queue");
  vr->matroska_muxer = gst_element_factory_make("matroskamux", "muxer");
  vr->app_sink = gst_element_factory_make("appsink", "app_sink");
  
  vr->pipeline = gst_pipeline_new("pipe_line");
  
  if (!vr->app_source ||
      !vr->video_queue ||
      !vr->matroska_muxer ||
      !vr->app_sink) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  g_signal_connect(vr->app_source,
    "need-data",
    G_CALLBACK(VideoRecorder_StartFeed),
    vr);
  g_signal_connect(vr->app_source,
    "enough-data",
    G_CALLBACK(VideoRecorder_StopFeed),
    vr);
  g_signal_connect(vr->app_sink,
    "new-sample", G_CALLBACK(VideoRecorder_SampleFeed), vr);
}

int32_t VideoRecorder_Close(VideoRecorder_t *vr) {
  gst_element_set_state(vr->pipeline, GST_STATE_NULL);
  gst_object_unref(vr->pipeline);
}


int32_t VideoRecorder_SetInputFormat(VideoRecorder_t *vr, Format_t *format) {
  GstVideoInfo info;
  GstCaps *video_caps;

  gst_video_info_set_format(&info, GST_VIDEO_FORMAT_RGB, format->width, format->height);
  video_caps = gst_video_info_to_caps(&info);
  g_object_set(vr->app_source,
    "caps", video_caps,
    "format", GST_FORMAT_TIME,
    NULL);
  g_object_set(vr->app_sink,
    "emit-signals", TRUE,
    "caps", video_caps,
    NULL);
  gst_caps_unref(video_caps);
}

int32_t VideoRecorder_SetOutputFormat(VideoRecorder_t *vr, Format_t *format) {}

int32_t VideoRecorder_StartStream(VideoRecorder_t *vr) {}

int32_t VideoRecorder_StopStream(VideoRecorder_t *vr) {}

int32_t VideoRecorder_SubmitFrame(VideoRecorder_t *vr, Image_t *image) {}

int32_t VideoRecorder_ReceiveOutStreamData(VideoRecorder_t *vr,
  Buffer_t *buffer) {}