
#ifndef DEPTHFILTER_H
#define DEPTHFILTER_H

#include "opencv2/core/core.hpp"

using namespace cv;

Mat depthFilter(Mat &depth, Mat &color, bool debug);

Mat FocalLengthAdaptFilter(Mat &color, Mat &depth, double focalLength, int defocusConst);

#endif