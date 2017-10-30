#ifndef OPENCV_PRECOMP_H
#define OPENCV_PRECOMP_H

#include <opencv2/opencv.hpp>
#include <algorithm>
#include <limits>

namespace cv
{

using std::vector;

class CV_EXPORTS_W MSER2
{
public:
  //! the full constructor
  CV_WRAP static Ptr<MSER2> create(int _delta = 5, int _min_area = 60, int _max_area = 14400, bool useBetter = true,
                                   double _max_variation = 0.25, double _min_diversity = .2,
                                   int _max_evolution = 200, double _area_threshold = 1.01,
                                   double _min_margin = 0.003, int _edge_blur_size = 5);

  CV_WRAP virtual void detectRegions(InputArray _src, vector<vector<Point> > &msers_blue, vector<Rect> &bboxes_blue,
                                     vector<vector<Point> > &msers_yellow, vector<Rect> &bboxes_yellow) = 0;
  CV_WRAP virtual void detectRegions(InputArray _src, vector<vector<Point> > &msers, vector<Rect> &bboxes, int type) = 0;

  CV_WRAP virtual void setDelta(int delta) = 0;
  CV_WRAP virtual int getDelta() const = 0;

  CV_WRAP virtual void setMinArea(int minArea) = 0;
  CV_WRAP virtual int getMinArea() const = 0;

  CV_WRAP virtual void setMaxArea(int maxArea) = 0;
  CV_WRAP virtual int getMaxArea() const = 0;

  CV_WRAP virtual void setPass2Only(bool f) = 0;
  CV_WRAP virtual bool getPass2Only() const = 0;
};
}
#endif