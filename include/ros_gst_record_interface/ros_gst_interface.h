#ifndef GST_ROS_RECORD_INTERFACE_H_
#define GST_ROS_RECORD_INTERFACE_H_

#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <gst/video/video.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
//extern "C" {
#endif
// Setup interface objects
typedef struct Queue_s {
  uint8_t **queue;
  uint8_t *read, *front;
  uint32_t size;
  uint32_t buffer_size;
} Queue_t;

void Queue_New(Queue_t *queue, uint32_t size, uint32_t buffer_size);

void Queue_Free(Queue_t *queue);

void Queue_Push(Queue_t *queue, uint8_t *data);

void Queue_PopCopy(Queue_t *queue, uint8_t *data);

void Queue_PopPointer(Queue_t *queue, uint8_t **data);


typedef struct _VideoRecorder_s {
  GstElement *pipeline;
  GstElement *app_source;
  GstElement *video_queue;
  GstElement *audio_queue;
  GstElement *matroska_muxer;
  GstElement *app_sink;
  uint64_t num_samples;

  Queue_t image_queue;

  guint sourceid;
  GMainLoop *main_loop;
} VideoRecorder_t;


typedef struct Format_s {
  uint32_t width, height;
  float frame_rate;
} Format_t;

typedef struct Image_s {
  uint8_t *data;
  uint32_t width, height;
  uint32_t channels;
  uint32_t bytes_per_pixel;
} Image_t;

typedef struct Buffer_s {
  uint8_t *data;
  uint32_t bytes;
} Buffer_t;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Functions //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int32_t VideoRecorder_Init(VideoRecorder_t *vr, int argc, char *argv[]);

int32_t VideoRecorder_Close(VideoRecorder_t *vr);

int32_t VideoRecorder_SetInputFormat(VideoRecorder_t *vr, Format_t *format);

int32_t VideoRecorder_SetOutputFormat(VideoRecorder_t *vr, Format_t *format);

int32_t VideoRecorder_StartStream(VideoRecorder_t *vr);

int32_t VideoRecorder_StopStream(VideoRecorder_t *vr);

int32_t VideoRecorder_SubmitFrame(VideoRecorder_t *vr, Image_t *image);

int32_t VideoRecorder_ReceiveOutStreamData(VideoRecorder_t *vr,
  Buffer_t *buffer);
#ifdef __cplusplus
//}
#endif

#endif  // GST_ROS_RECORD_INTERFACE_H_
