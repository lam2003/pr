#include "judger.h"

Judger *Judger::instance = NULL;

Judger *Judger::getInstance()
{
    if(instance == NULL)
        instance = new Judger();
    return instance;
}

Judger::Judger()
{
    svm_ptr = SVM::load(kSVMModelPath);
}


bool Judger::judgePlate(CPlate &cplate,float thresh)
{
    Mat cplate_mat = cplate.getMat();
    Mat feature_mat = svmFeatures(cplate_mat);
    float score = svm_ptr->predict(feature_mat,noArray(),StatModel::Flags::RAW_OUTPUT);
    score = 1.f-score;
    cplate.setScore(score);

    if(score < thresh)
    {
        int width = cplate_mat.cols;
        int height = cplate_mat.rows;

        Mat temp_mat = cplate_mat(Rect_<float>(width * 0.05f,height * 0.1f,width * 0.9f,height * 0.8f));
        resize(temp_mat,temp_mat,Size(cplate_mat.size()));
        feature_mat = svmFeatures(temp_mat);
      
        score = svm_ptr->predict(feature_mat,noArray(),StatModel::Flags::RAW_OUTPUT);
        score = 1.f - score;
        cplate.setScore(score);
    }

    if(score > thresh)
        return true;
    else 
        return false;
}

void Judger::judgePlate(vector<CPlate> &cplate_vec,float thresh)
{
    float overlap = 0.5f;
    notMaxSuppression(cplate_vec,overlap);

    for(int i = 0; i < cplate_vec.size(); i++)
        judgePlate(cplate_vec[i],thresh);
    
}

