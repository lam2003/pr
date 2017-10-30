#ifndef JUDGER_H
#define JUDGER_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "functions.h"
#include "feature.h"
#include "global.h"
#include "cplate.h"
using namespace cv;
using namespace std;

class Judger
{
public:
    static Judger *getInstance();
    bool judgePlate(CPlate &cplate,float thresh);
    void judgePlate(vector<CPlate> &cplate_vec,float thresh);
private:
    Judger();
    static Judger *instance;
    Ptr<SVM> svm_ptr;
};

#endif 