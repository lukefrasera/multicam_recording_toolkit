#include "ros_gst_record_interface/ros_gst_interface.h"
#include "stdio.h"
#include "stdlib.h"
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <glib.h>

#define CHUNK_SIZE 1024

static gboolean VideoRecorder_PushData(VideoRecorder_t *vr) {
  GstBuffer *buffer;
  GstMemory *memory;
  //uint64_t size = CHUNK_SIZE;
  uint64_t size = vr->format.width * vr->format.height * 3;
  GstFlowReturn flow_return;

  buffer = gst_buffer_new();
  memory = gst_allocator_alloc(NULL, size, NULL);

  gst_buffer_insert_memory(buffer, -1, memory);
  // Set its timestamp and duration
  GST_BUFFER_DTS(buffer) = gst_clock_get_time(vr->time);
  GST_BUFFER_PTS(buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_OFFSET(buffer) = vr->frame_num;
  vr->frame_num++;
  
  g_usleep(1000000/2);
  // Push buffer into appsrc
  g_signal_emit_by_name(vr->app_source, "push-buffer", buffer, &flow_return);
  //gst_app_src_push_buffer((GstAppSrc*)vr->app_source, buffer);
  g_print("Adding Data to the appsrc queue\n");
  gst_buffer_unref(buffer);
  if (flow_return != GST_FLOW_OK) {
    return -1;
  }
  return 1;
}

static void VideoRecorder_StartFeed(GstElement *source, guint size, VideoRecorder_t *vr) {
  if (vr->sourceid == 0) {
    g_print("Start Feeding\n");
    vr->sourceid = g_idle_add((GSourceFunc)VideoRecorder_PushData, vr);
  }
}

static void VideoRecorder_StopFeed(GstElement *source, VideoRecorder_t *vr) {
  if (vr->sourceid !=0) {
    g_print("Stopping Feed\n");
    g_source_remove(vr->sourceid);
    vr->sourceid = 0;
  }
}

static void VideoRecorder_SampleFeed(GstElement *sink, VideoRecorder_t *vr) {
  GstSample *sample;

  g_signal_emit_by_name(sink, "pull-sample", &sample);
  if (sample) {
    g_print("*");
    gst_sample_unref(sample);
  }
}

int32_t VideoRecorder_Init(VideoRecorder_t *vr, int argc, char *argv[]) {
  gst_init(&argc, &argv);
  memset(vr, 0, sizeof(VideoRecorder_t));
  vr->time = gst_system_clock_obtain();
  vr->app_source     = gst_element_factory_make("appsrc", "video_source");
  vr->video_queue    = gst_element_factory_make("queue", "video_queue");
  vr->matroska_muxer = gst_element_factory_make("matroskamux", "muxer");
  vr->app_sink       = gst_element_factory_make("appsink", "app_sink");
  
  vr->pipeline = gst_pipeline_new("pipe_line");
  
  if (!vr->app_source     ||
      !vr->video_queue    ||
      !vr->matroska_muxer ||
      !vr->app_sink) {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }
  
  //g_object_set(vr->matroska_muxer, "streamable", TRUE, NULL);
  g_object_set(vr->app_source,"format", GST_FORMAT_TIME, NULL);
  //g_object_set(vr->app_source, "block", G_TYPE_BOOLEAN, TRUE, NULL);
  gst_app_src_set_max_bytes((GstAppSrc*)vr->app_source, 3 * vr->format.height * vr->format.width * 1);
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
  return 1;
}

int32_t VideoRecorder_Close(VideoRecorder_t *vr) {
  gst_element_set_state(vr->pipeline, GST_STATE_NULL);
  gst_object_unref(vr->pipeline);
  return 1;
}


int32_t VideoRecorder_SetInputFormat(
    VideoRecorder_t *vr, Format_t *format) {
  GstVideoInfo info;
  GstCaps *video_caps;
  GstPadTemplate *pad_template;
  GstPad * matroska_video_pad;
  GstPad *video_queue_pad;
  
  memcpy(&vr->format, format, sizeof(Format_t));

  gst_video_info_set_format(&info,
    GST_VIDEO_FORMAT_RGB, format->width, format->height);
  //video_caps = gst_video_info_to_caps(&info);
  video_caps = gst_caps_new_simple("video/x-raw",
    "format", G_TYPE_STRING, "RGB",
    "width", G_TYPE_INT, 640,
    "height", G_TYPE_INT, 480,
    "framerate", GST_TYPE_FRACTION, 30, 1,
    "pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1,
    NULL);
  g_object_set(vr->app_source,
    "caps", video_caps,
    "format", GST_FORMAT_TIME,
    NULL);
  //g_object_set(vr->app_sink,
  //  "emit-signals", TRUE,
  //  "caps", video_caps,
  //  NULL);

  // Request Matroska Mux Sinks
  pad_template       = gst_element_class_get_pad_template(
      GST_ELEMENT_GET_CLASS(vr->matroska_muxer),
      "video_%u");
  matroska_video_pad = gst_element_request_pad(
      vr->matroska_muxer,
      pad_template,
      NULL,
      video_caps);
  g_print ("Obtained request pad [%s] video pipeline.\n",
      gst_pad_get_name(matroska_video_pad));

  // Link Video pad with Video queue src
  video_queue_pad = gst_element_get_static_pad (vr->app_source, "src");
  if (gst_pad_link(video_queue_pad, matroska_video_pad) != GST_PAD_LINK_OK) {
    g_printerr("Video Pad could not be linked.\n");
    gst_object_unref(vr->pipeline);
    return -1;
  }

  gst_object_unref(video_queue_pad);
  
  gst_caps_unref(video_caps);
  return 1;
}

/* Prints information about a Pad Template, including its Capabilities */
static void print_pad_templates_information (GstElementFactory * factory) {
  const GList *pads;
  GstStaticPadTemplate *padtemplate;
   
  g_print ("Pad Templates for %s:\n", gst_element_factory_get_longname (factory));
  if (!gst_element_factory_get_num_pad_templates(factory)) {
    g_print ("  none\n");
    return;
  }
   
  pads = gst_element_factory_get_static_pad_templates(factory);
  while (pads) {
    padtemplate = (GstStaticPadTemplate *) (pads->data);
    pads = g_list_next (pads);
     
    if (padtemplate->direction == GST_PAD_SRC)
      g_print ("  SRC template: '%s'\n", padtemplate->name_template);
    else if (padtemplate->direction == GST_PAD_SINK)
      g_print ("  SINK template: '%s'\n", padtemplate->name_template);
    else
      g_print ("  UNKNOWN!!! template: '%s'\n", padtemplate->name_template);
     
    if (padtemplate->presence == GST_PAD_ALWAYS)
      g_print ("    Availability: Always\n");
    else if (padtemplate->presence == GST_PAD_SOMETIMES)
      g_print ("    Availability: Sometimes\n");
    else if (padtemplate->presence == GST_PAD_REQUEST) {
      g_print ("    Availability: On request\n");
    } else
      g_print ("    Availability: UNKNOWN!!!\n");
     
    if (padtemplate->static_caps.string) {
      g_print ("    Capabilities:\n");
      //print_caps (gst_static_caps_get (&padtemplate->static_caps), "      ");
      GST_LOG("caps are %" GST_PTR_FORMAT, gst_static_caps_get(&padtemplate->static_caps));
    }
     
    g_print ("\n");
  }
}

int32_t VideoRecorder_SetOutputFormat(
    VideoRecorder_t *vr,
    Format_t *format) {
  GstVideoInfo info;
  GstCaps *video_caps;
  GstPad *video_src_pad;
  GstElementFactory *factory;
  // Setup raw output format by default
  //gst_video_info_set_format(&info, 
  factory = gst_element_factory_find("matroskamux");
  print_pad_templates_information(factory);

  //video_caps = gst_caps_new_simple("video/x-matroska",
  //  "framerate", GST_TYPE_FRACTION, 30, 1, NULL);
  
  // Get src pad ov matroska mux
  video_src_pad = gst_element_get_static_pad(vr->matroska_muxer, "src");
  video_caps = gst_pad_get_current_caps(video_src_pad);
  g_object_set(vr->app_sink,
    "emit-signals", TRUE,
    "caps", video_src_pad->padtemplate->caps,
    NULL);
  g_print("caps are %s\n", gst_caps_to_string(video_src_pad->padtemplate->caps));
  //gst_object_unref(video_src_pad);
  //gst_object_unref(video_caps);
  gst_app_sink_set_emit_signals((GstAppSink*)vr->app_sink, TRUE);
  if (!gst_app_sink_get_emit_signals((GstAppSink*)vr->app_sink)) {
    g_print("Error: Won't output signals!\n");
    return -1;
  }
  if (gst_element_link_many(
      vr->matroska_muxer,
      vr->app_sink,
      NULL) != TRUE) {
    g_printerr("Elements could not be linked!\n");
    gst_object_unref(vr->pipeline);
    return -1;
  }

  return 1;
}
void error_cb(GstBus *bus, GstMessage *msg, VideoRecorder_t * vr) {
  GError *err;
  gchar *debug_info;

  gst_message_parse_error(msg, &err, &debug_info);
  g_printerr("Error received from element %s: %s\n",
    GST_OBJECT_NAME(msg->src), err->message);
  g_printerr("Debugging information: %s\n",
    debug_info ? debug_info : "none");
  g_clear_error(&err);
  g_free(debug_info);

  g_main_loop_quit(vr->main_loop);
}
int32_t VideoRecorder_InitPipeline(VideoRecorder_t *vr) {
  GstBus *bus;
  gst_bin_add_many(GST_BIN(vr->pipeline),
    vr->app_source,
    vr->video_queue,
    vr->matroska_muxer,
    vr->app_sink,
    NULL);

  // Link Pipeline together
  if (gst_element_link(
      vr->app_source,
      vr->matroska_muxer) != TRUE) {
    g_printerr("Elements could not be linked!\n");
    gst_object_unref(vr->pipeline);
    return -1;
  }

  bus = gst_element_get_bus(vr->pipeline);
  gst_bus_add_signal_watch(bus);
  g_signal_connect(G_OBJECT(bus),
    "message::error",
    (GCallback)error_cb, vr);
  return 1;
}

int32_t VideoRecorder_StartStream(VideoRecorder_t *vr) {
  GstSample *sample;
  gst_element_set_state(vr->pipeline, GST_STATE_PLAYING);
  vr->main_loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(vr->main_loop);
  
  // Try to pull a sample from the appsink
  sample = gst_app_sink_pull_sample((GstAppSink*)vr->app_sink);
  g_print("Received Sample!\n");
  if (sample) {

    g_print("Sample is real\n");
    gst_sample_unref(sample);
  }
  return 1;
}

int32_t VideoRecorder_StopStream(VideoRecorder_t *vr) {}

int32_t VideoRecorder_SubmitFrame(VideoRecorder_t *vr, Image_t *image) {}

int32_t VideoRecorder_ReceiveOutStreamData(VideoRecorder_t *vr,
  Buffer_t *buffer) {}

