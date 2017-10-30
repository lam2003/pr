#ifndef CPLATE_H
#define CPLATE_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "cchar.h"
#include "global.h"

using namespace cv;
using namespace std;

class CPlate
{
public:
  CPlate()
  {
    score = -2.f;
    label = "";
    color = UNKNOWN;
  }
  ~CPlate() {}

  inline void setColor(Color color) { this->color = color; }
  inline Color getColor() { return color; }

  inline void setLeftPoint(Point left_point) { this->left_point = left_point; }
  inline Point getLeftPoint() const { return left_point; }

  inline void setRightPoint(Point right_point) { this->right_point = right_point; }
  inline Point getRightPoint() const { return right_point; }

  inline void setLineVec4f(Vec4f line_vec4f) { this->line_vec4f = line_vec4f; }
  inline Vec4f getLineVec4f() const { return line_vec4f; }

  inline void setOtsuLevel(int otsu_level) { this->otsu_level = otsu_level; }
  inline int getOtsuLevel() const { return otsu_level; }

  inline void setDistVec2i(Vec2i dist_vec2i) { this->dist_vec2i = dist_vec2i; }
  inline Vec2i getDistVec2i() const { return dist_vec2i; }

  inline void setRect(Rect rect) { this->rect = rect; }
  inline Rect getRect() const { return rect; }

  inline void setMaxCCharRect(Rect max_cchar_rect) { this->max_cchar_rect = max_cchar_rect; }
  inline Rect getMaxCCharRect() const { return max_cchar_rect; }

  inline void setMserCCharVec(vector<CChar> &mser_cchar_vec) { this->mser_cchar_vec = mser_cchar_vec; }
  inline vector<CChar> getCopyOfMserCCharVec() const { return mser_cchar_vec; }

  inline void setRRect(RotatedRect rrect) { this->rrect = rrect; }
  inline RotatedRect getRRect() const{ return rrect; }

  inline void setIsPlate(bool is_plate) { this->is_plate = is_plate; }
  inline bool getIsPlate() const{return is_plate; }

  inline void setMat(Mat mat) { this->mat = mat; }
  inline Mat getMat() const{return mat; }

  inline void setScore(float score) {this->score = score;}
  inline float getScore() const{ return score; }

  inline void setMaskMat(Mat mask_mat) {this->mask_mat = mask_mat;}
  inline Mat getMaskMat() const{return mask_mat;}

  inline void setBoundRect(Rect_<float> bound_rect) {this->bound_rect = bound_rect;}
  inline Rect_<float> getBoundRect() const{return bound_rect;}

  inline void setBoundMat(Mat bound_mat){this->bound_mat = bound_mat;}
  inline Mat getBoundMat() const{return bound_mat;}
private:
  Mat mat;
  Mat mask_mat;
  RotatedRect rrect;
  Rect_<float> bound_rect;
  Mat bound_mat;
  string label;
  float score;
  float otsu_level;
  Point left_point;
  Point right_point;
  Rect rect;
  Rect max_cchar_rect;
  Vec2i dist_vec2i;
  Vec4f line_vec4f;
  vector<CChar> mser_cchar_vec;
  Color color;
  bool is_plate;
};

#endif