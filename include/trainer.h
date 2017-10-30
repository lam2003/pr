#ifndef TRAINER_H
#define TRAINER_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "functions.h"
#include "utils.h"
#include "global.h"
#include "feature.h"

using namespace std;
using namespace cv;
using namespace cv::ml;


class ANNTrainer
{
public:
    ANNTrainer(int type);
    void train();
    void test(); 
private:
    int identify(const Mat &in);
    Ptr<TrainData> preprocessTrainData(int min_sample_num);
    Ptr<ANN_MLP> ann_ptr;
    int type;
    vector<Mat> test_images;
    vector<int> test_labels;
};

class SVMTrainer
{
public:
    SVMTrainer();
    void train();
    void test();
private:
    bool judge(const Mat &in);
    vector<Mat> test_images;
    vector<int> test_labels;
    Ptr<SVM> svm_ptr;
    Ptr<TrainData> preprocessTrainData();
};
#endif 