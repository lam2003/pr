#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "cplate.h"
#include "cchar.h"
#include "global.h"
#include "identifier.h"

using namespace cv;
using namespace std;

bool compareCPlateByRectTlX(const CPlate &cplate1,const CPlate &cplate2);
bool compareCCharByScore(const CChar &cchar1, const CChar &cchar2);
bool compareCCharByCenterX(const CChar &cchar1,const CChar &cchar2);
bool compareCCharByRectTlX(const CChar &cchar1,const CChar &cchar2);
bool compareVec2iByX(const Vec2i &vec2i1,const Vec2i &vec2i2);
bool compareCCharByRectTlXGD(const CChar &cchar1,const CChar &cchar2);
Mat getSyntheticMat(const Mat &in,int type);
int getBorderColor(const Mat &in);
Rect getCenterRect(const Mat &in);
Mat getRectMat(const Mat &in, const Rect &rect);
Rect getSafeRect(const Mat &in,const Point2f &center_point,float width,float height);
Rect_<float> getSafeRect(const Mat &in,const RotatedRect &rrect);
//flag == 0,计算每列的均衡化
//flag == 1,计算每行的均衡化
Mat projectedHistogram(const Mat &in, int flag, int thresh = 20);
Mat getTranslatedMat(const Mat &in, float x_offset, float y_offset,int bg_color = 0);
Mat getRotatedMat(const Mat &in, float angle,int bg_color = 0);

Mat scaleImage(const Mat &in, const Size &size,float &scale_ratio);
bool verifyCharSizes(const Rect &rect);
bool verifyPlateSizes(const RotatedRect &rrect);
Mat paintImageByPoints(const vector<Point> &point_vec,const Rect &rect);
Mat preprocessChar(const Mat &in, int char_size);

//使字符矩形的长宽比变为90 / 45
Rect adaptiveCharRect(const Rect &rect,int max_width);

//对字符'1'等长宽比大于3的字符进行排查,如果扩大搜索后二值图发生变化,说明图像不是字符
bool judgeMserCharDiffRatio(const Mat &in, const Rect &rect, float thresh = 0.1);

//清除重叠的字符
void notMaxSuppression(vector<CChar> &cchar_vec, float overlap);
void notMaxSuppression(vector<CPlate> &cplate_vec, float overlap);

//计算inter_rect.area() / merge_rect.area()
float calcInsertOverUnion(const Rect &rect1,const Rect &rect2);

//将y轴相似,距离适当的强字符归为一组
void mergeCharToGroup(const vector<CChar> &cchar_vec, vector<vector<CChar> > &cchar_group_vec);

//去除字符组中脱离斜率直线的字符
void removeRightOutliers(vector<CChar> &cchar_vec,vector<CChar> &out_cchar_vec,float min_thresh,float max_thresh);

//去除相包含的字符
void removeContainChar(vector<CChar> &cchar_vec,float thresh);

//在同一斜率的轴线上搜索字符
void axesSearch(const Vec4f &cplate_line_vec4f,
     Point &cplate_left_point,
     Point &cplate_right_point,
     Rect &cplate_max_cchar_rect,
     Rect &cplate_rect,
     vector<CChar> &cchar_vec,
     vector<CChar> &out_cchar_vec,
     float thresh,
     int flag);

void slideWindowSearch(const Mat &in,
     const Vec4f &cplate_line_vec4f,
     Point &cplate_left_point,
     Point &cplate_right_point,
     const Rect &cplate_max_cchar_rect,
     Rect &cplate_rect,
     vector<CChar> &out_cchar_vec,
     float cplate_otsu_level,
     float window_ratio,
     float thresh,
     bool is_chinese,
     int flag);


void combineRect(const Mat &in, 
     vector<CChar> &cchar_vec,
     vector<CChar> &out_cchar_vec,
     const Vec2i &cplate_dest_vec2i,
     const Rect &cplate_max_cchar_rect, 
     float min_thresh, 
     float max_thresh);

Mat affine(const Mat &in,float slope);
bool isDeflection(const Mat &in,float angle,float &slope);
Mat getRotatedMat(const Mat &in,const Size &size,const Point2f &center_point,float angle);
void deskew(CPlate &cplate);
void enlargeCCharRect(const Mat &in,CChar &cchar,const Vec4f &cplate_line_vec4f,const Rect &cplate_max_cchar_rect);
#endif