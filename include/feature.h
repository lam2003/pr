#ifndef FEATURE_H
#define FEATURE_H

#include <opencv2/opencv.hpp>
#include "functions.h"
using namespace cv;

Mat getProjectedMat(const Mat &in,int width,int height);
Mat getColorProjectedMat(const Mat &in);
Mat annFeatures(const Mat &in,int width,int height,int type);
Mat svmFeatures(const Mat &in);
#endif