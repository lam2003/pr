#include "pr.h"

int main(int argc, char *argv[])
{

    vector<string> files;
    lsDir(argv[1], files);
    for (int i = 0; i < files.size(); i++)
    {
        Mat in = imread(files[i]);

        ostringstream oss;
        oss << i << ".jpg";

        Locater locater(in);
        float scale_ratio;
        in = scaleImage(in,Size(1000,1000),scale_ratio);
        locater.setDisplay(oss.str());
        locater.mserCharLocated();
    }
/* 
    ANNTrainer ann_trainer(1);
    ann_trainer.train();
    ann_trainer.test();  */


 /*    SVMTrainer svm_trainer;
    svm_trainer.train();
    svm_trainer.test();
    return 0; */
}