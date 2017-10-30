#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <opencv2/opencv.hpp>
#include "global.h"
#include "feature.h"
#include "cchar.h"

using namespace cv;
using namespace cv::ml;

class CharIdentifier
{
public:
  static CharIdentifier *getInstance();
  void classify(vector<CChar> &char_vec);
  void classifyChinese(vector<CChar> &char_vec);
  bool isChinese(CChar &cchar,float thresh);

  ~CharIdentifier();

private:
  CharIdentifier();
  static CharIdentifier *instance;
  Ptr<ANN_MLP> ann_ptr;
  Ptr<ANN_MLP> ann_chinese_ptr;
  Ptr<ANN_MLP> ann_chinese_gray_ptr;
};
#endif