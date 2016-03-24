#!/usr/env/bin bash

VIDEO_DEVICE=/dev/video0
VIDEO_CAPABILITIES="video/x-raw, format=BGR, width=1280, height=720, pixel-aspect-ratio=1/1, framerate=30/1"

gst-launch-1.0 v4l2src device=$VIDEO_DEVICE ! $VIDEO_CAPABILITIES ! fpsdisplaysink fps-update-interval=100000
