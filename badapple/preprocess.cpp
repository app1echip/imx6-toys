#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;
#define FRMS 6572

int main() {
  VideoCapture vid("16x16_bin.mp4");
  FILE *fp = fopen("data", "wb");
  unsigned short fms[FRMS][16];
  Mat mat;
  vid >> mat;
  int fid = 0;
  while (!mat.empty()) {
    cvtColor(mat, mat, COLOR_BGR2GRAY);
    threshold(mat, mat, 0, UCHAR_MAX, THRESH_BINARY | THRESH_OTSU);
    for (int i = 0; i < 16; ++i) {
      fms[fid][i] = 0;
      for (int j = 0; j < 16; ++j) {
        unsigned char on = (mat.at<uchar>(15 - j, i) == 0 ? 0 : 1);
        fms[fid][i] += (on << (15 - j));
      }
      fwrite(&fms[fid][i], sizeof(fms[fid][i]), 1, fp);
    }
    vid >> mat;
    fid++;
  }
  fclose(fp);
  return 0;
}
