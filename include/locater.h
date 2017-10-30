#ifndef LOCATER_H
#define LOCATER_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "cplate.h"
#include "mser2.h"
#include "global.h"
#include "identifier.h"
#include "judger.h"
#include "utils.h"

using namespace cv;
using namespace std;

#define RECTANGLE(mat, cchar_vec, color)                      \
    do                                                        \
    {                                                         \
        int i = 0;                                            \
        for (i = 0; i < cchar_vec.size(); i++)                \
            rectangle(mat, cchar_vec[i].getRect(), color, 1); \
    \
} while (0)

class Locater
{
  public:
    Locater(const Mat &in);
    void setDisplay(const string save_name);
    void mserCharLocated();
    inline vector<CPlate> getCPlateVec() { return cplate_vec; }

  private:    
    void preprocessToSegment();
    vector<CPlate> cplate_vec;
    Mat mat;
    bool display;
    string save_name;
};

#endif