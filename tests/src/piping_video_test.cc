#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <pstreams/pstream.h>
#include <fstream>


int main(int argc, char *argv[]) {
  cv::VideoCapture cap(0);
  std::ofstream fout("test.txt");

  if (!cap.isOpened()) {
    printf("Error openning Capture device!\n");
    return -1;
  }
  redi::opstream pout("ffmpeg -f rawvideo -pix_fmt bgr24 -s 640x480 -r 30 -i - -an -c:v mpeg4 -qscale:v 30 -y -f matroska output.mkv");
  uint8_t *data;
  cv::Mat frame;
  for (int i = 0; i < 20 * 30; ++i) {
    cap >> frame;
    data = (uint8_t*)frame.data;
    // send data to ffmpeg process
    //  OPENCV stored images in the BGR format

    pout.write((char*)data, 640*480*3);
  }
  return 0;
}