#!/usr/bin/env bash

# TV_NORM=

VIDEO_DEVICE=/dev/video0
VIDEO_CAPABILITIES="video/x-raw, format=BGR, width=1280, height=720, pixel-aspect-ratio=1/1, framerate=30/1"
ENCODE_VIDEO_FORMAT="mpeg4"
ENCODE_VIDEO_OPTION="-b:v 10000k"

AUDIO_DEVICE='hw:CARD=PCH,DEV=0'
AUDIO_CAPABILITIES=""
ENCODE_AUDIO_FORMAT=""
ENCODE_AUDIO_OPTION=""

ENCODE_MUXER_FORMAT="matroska"
ENCODE_MUXER_OPTION=""
ENCODE_FILENAME="test.mkv"


ffmpeg \
    -i <(
         gst-launch-1.0 -q \
            v4l2src device=$VIDEO_DEVICE do-timestamp=true pixel-aspect-ratio=1 \
                ! $VIDEO_CAPABILITIES \
                ! queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 \
                ! mux. \
            alsasrc device=$AUDIO_DEVICE do-timestamp=true \
                ! queue max-size-buffers=0 max-size-time=0 max-size-bytes=0 \
                ! mux. \
            matroskamux name=mux \
                ! fdsink fd=1
    ) \
    -c:v $ENCODE_VIDEO_FORMAT $ENCODE_VIDEO_OPTION \
    -f   $ENCODE_MUXER_FORMAT $ENCODE_MUXER_OPTION $ENCODE_FILENAME
    # -c:a $ENCODE_AUDIO_FORMAT $ENCODE_AUDIO_OPTIONS \