#ifndef DETECT_H_
#define DETECT_H_

#include <iostream>
#include <opencv2/core.hpp>

#include "detect.cpp"

using namespace std;
using namespace cv;

int detect_corners(string, Point2i*, char*);

#endif
