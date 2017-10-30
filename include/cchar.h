#ifndef CCHAR_H
#define CCHAR_H

#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;
class CChar
{
  public:
    CChar()
    {
        score = 0.f;
        index = 0;
        label = "";
        otsu_level = 125;
        is_chinese = false;
    }
    ~CChar(){}

    inline void setMat(Mat mat) { this->mat = mat.clone(); }
    inline Mat getMat() const { return mat; }

    inline void setGrayMat(Mat gray_mat) { this->gray_mat = gray_mat.clone(); }
    inline Mat getGrayMat() const { return gray_mat; }

    inline void setRect(Rect rect) { this->rect = rect; }
    inline Rect getRect() const { return rect; }

    inline void setLabel(string label) { this->label = label; }
    inline string getLabel() const { return label; }

    inline void setScore(float score) { this->score = score; }
    inline float getScore() const { return score; }

    inline void setOtsuLevel(float otsu_level) { this->otsu_level = otsu_level; }
    inline float getOtsuLevel() const { return otsu_level; }

    inline void setIsChinese(bool is_chinese) { this->is_chinese = is_chinese; }
    inline bool getIsChinese() const { return is_chinese; }

    inline void setIndex(int index) { this->index = index; }
    inline int getIndex() const { return index; }

    inline void setCenterPoint(Point center_point) { this->center_point = center_point; }
    inline Point getCenterPoint() const { return center_point; }

    inline bool getIsStrong() const { return score >= 0.9f; }
  private:
    Mat mat;
    Mat gray_mat;
    Rect rect;
    string label;
    float score;
    float otsu_level;
    Point center_point;
    int index;
    bool is_chinese;
};

#endif