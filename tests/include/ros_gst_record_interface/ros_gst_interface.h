#ifndef GST_ROS_RECORD_INTERFACE_H_
#define GST_ROS_RECORD_INTERFACE_H_
#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <gst/video/video.h>
#include <string.h>

#ifdef __cplusplus
//extern "C" {
#endif
// Setup interface objects
typedef struct _CustomData {
  GstElement *pipeline, *app_source, *tee, *audio_queue, *audio_convert1, *audio_resample, *audio_sink;
  GstElement *video_queue, *audio_convert2, *visual, *video_convert, *video_sink;
  GstElement *app_queue, *app_sink;
  
  guint64 num_samples;   /* Number of samples generated so far (for timestamp generation) */
  
  guint sourceid;        /* To control the GSource */
  
  GMainLoop *main_loop;  /* GLib's Main Loop */
} CustomData;


typedef struct _VideoRecorder_s {
  GstElement *pipeline;
  GstElement *app_source;
  GstElement *video_queue;
  GstElement *audio_queue;
  GstElement *matroska_muxer;
  GstElement *app_sink;
  uint64_t num_samples;

  guint sourceid;
  GMainLoop *main_loop;
} VideoRecorder_t;


typedef struct Format_s {
  int fmt;
} Format_t;

typedef struct Image_s {
  uint8_t *data;
  uint32_t width, height;
  uint32_t channels;
  uint32_t bytes_per_pixel;
} Image_t;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Functions //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int32_t VideoRecorder_Init(VideoRecorder_t *vr);

int32_t VideoeRcorder_SetInputFormat(VideoRecorder_t *vr, Format_t *format);

int32_t VideoRecorder_SetOutputFormat(VideoRecorder_t *vr, Format_t *format);

int32_t VideoRecorder_StartStream(VideoRecorder_t *vr);

int32_t VideoRecorder_StopStream(VideoRecorder_t *vr);

int32_t VideoRecorder_SubmitFrame(VideoRecorder_t *vr, Image_t *image);
#ifdef __cplusplus
//}
#endif

#endif  // GST_ROS_RECORD_INTERFACE_H_
