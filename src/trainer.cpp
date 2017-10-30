#include "trainer.h"


ANNTrainer::ANNTrainer(int type)
{
    ann_ptr = ANN_MLP::create();
    this->type = type;
}


Ptr<TrainData> ANNTrainer::preprocessTrainData(int min_sample_num)
{
    int n = 0;
    Mat train_images;
    vector<int> train_labels;
    srand(unsigned(time(NULL)));

    if (type == 0)
        n = kCharsTotalNumber;
    else
        n = kChineseNumber;
    for (int i = 0; i < n; i++)
    {
        const char *char_dir = kChars[kCharsTotalNumber - n + i];
        char path[512] = {0};
        vector<string> file_paths;
        vector<Mat> input_images;

        if (type == 0)
            sprintf(path, "%s/%s", kANNSamplePath, char_dir);
        else if (type == 1)
            sprintf(path, "%s/%s", kANNZhSamplePath, char_dir);
        else if (type == 2)
            sprintf(path, "%s/%s", kANNZhGraySamplePath, char_dir);
        lsDir(path, file_paths);

        fprintf(stdout, "%s\n", path);

        for (int j = 0; j < file_paths.size(); j++)
        {
            Mat sample_mat = imread(file_paths[j].c_str(), 0);
            if (sample_mat.empty())
                continue;
            input_images.push_back(sample_mat);
        }

        int input_num = file_paths.size();
        for (int j = 0; j < min_sample_num - input_num; j++)
        {
            Mat sample_mat = input_images.at(rand() % (input_num + j));
            Mat synthetic_mat = getSyntheticMat(sample_mat,type);
        
            input_images.push_back(synthetic_mat);
        
            char synthetic_path[512] = {0};
            if(kANNSaveSynthetic)
            {
                if(type == 0)
                    sprintf(synthetic_path,"%s/%s-%d.jpg",kANNSyntheticPath,char_dir,j);
                else if(type == 1)
                    sprintf(synthetic_path,"%s/%s-%d.jpg",kANNZhSyntheticPath,char_dir,j);
                else if(type == 2)
                    sprintf(synthetic_path,"%s/%s-%d.jpg",kANNZhGraySyntheticPath,char_dir,j);
                imwrite(synthetic_path,synthetic_mat);
            }
        }

        for (int j = 0; j < input_images.size(); j++)
        {
            Mat sample_mat = input_images[j];

            test_images.push_back(sample_mat);
            test_labels.push_back(i);

            Mat feature_mat;
       
            if (type == 0)
                feature_mat = annFeatures(sample_mat, kANNDataSize, kANNDataSize, 0);
            else if (type == 1)
                feature_mat = annFeatures(sample_mat, kANNZhDataSize, kANNZhDataSize, 0);
            else if (type == 2)
                feature_mat = annFeatures(sample_mat, kANNZhGrayWidth, kANNZhGrayHeight, 1);

            train_images.push_back(feature_mat);
            train_labels.push_back(i);
        }
    }

  
    train_images.convertTo(train_images, CV_32F);
 
    Mat result_mat = Mat::zeros(train_labels.size(), n, CV_32F);

    for (int i = 0; i < result_mat.rows; i++)
        result_mat.at<float>(i, train_labels[i]) = 1.f;

    return TrainData::create(train_images, ROW_SAMPLE, result_mat);
}

void ANNTrainer::train()
{
    int input_num = 0;
    int hidden_num = 0;
    int output_num = 0;

    if (type == 0)
    {
        output_num = kCharsTotalNumber;
        input_num = kANNDataSize + kANNDataSize + kANNDataSize * kANNDataSize;
        hidden_num = 40;
    }
    else if (type == 1)
    {
        output_num = kChineseNumber;
        input_num = kANNZhDataSize + kANNZhDataSize + kANNZhDataSize * kANNZhDataSize;
        hidden_num = 40;
    }
    else if (type == 2)
    {
        output_num = kChineseNumber;
        input_num = kANNZhGrayWidth * kANNZhGrayHeight + kANNZhGrayWidth + kANNZhGrayWidth;
        hidden_num = 64;
    }
    else
        return;

    Mat layers(1, 3, CV_32SC1);
    layers.at<int>(0) = input_num;
    layers.at<int>(1) = hidden_num;
    layers.at<int>(2) = output_num;

    ann_ptr->setLayerSizes(layers);
    ann_ptr->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
    ann_ptr->setTrainMethod(ANN_MLP::BACKPROP);
    if(type == 0)
        ann_ptr->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 1000, 0.01));
    else
        ann_ptr->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.0001));

    ann_ptr->setBackpropWeightScale(0.1);
    ann_ptr->setBackpropMomentumScale(0.1);

    Ptr<TrainData> train_data;
    if(type == 0)
        train_data = preprocessTrainData(kANNMinSampleNum);
    else if(type == 1)
        train_data = preprocessTrainData(kANNZhMinSampleNum);
    else
        train_data = preprocessTrainData(kANNZhGrayMinSampleNum);

    fprintf(stdout, "++++++++++++++++++++++++++++++++++++++++\n");
    fprintf(stdout, "start train\n");
    ann_ptr->train(train_data);
    fprintf(stdout, "end train\n");
    fprintf(stdout, "++++++++++++++++++++++++++++++++++++++++\n");

    if (type == 0)
        ann_ptr->save(kANNModelPath);
    else if (type == 1)
        ann_ptr->save(kANNZhModelPath);
    else
        ann_ptr->save(kANNZhGrayModelPath);
}

int ANNTrainer::identify(const Mat &in)
{
    Mat feature_mat;
    Mat result_mat;
    int n;
    if (type == 0)
    {
        n = kCharsTotalNumber;
        feature_mat = annFeatures(in, kANNDataSize, kANNDataSize, 0);
        result_mat = Mat(1, kCharsTotalNumber, CV_32FC1);
    }
    else if (type == 1)
    {
        n = kChineseNumber;
        feature_mat = annFeatures(in, kANNZhDataSize, kANNZhDataSize, 0);
        result_mat = Mat(1, kChineseNumber, CV_32FC1);
    }
    else if (type == 2)
    {
        n = kChineseNumber;
        feature_mat = annFeatures(in, kANNZhGrayWidth, kANNZhGrayHeight, 1);
        result_mat = Mat(1, kChineseNumber, CV_32FC1);
    }
    float max_val = -2.f;
    int result = 0;

    ann_ptr->predict(feature_mat, result_mat);
 
    for (int i = 0; i < n; i++)
    {
        float val = result_mat.at<float>(i);
        if (val > max_val)
        {
            max_val = val;
            result = i;
        }
    }
    return result;
}

void ANNTrainer::test()
{
    int n = 0;
    int correct_sum = 0;
    int total_sum = 0;

    if (type == 0)
        n = kCharsTotalNumber;
    
    else if (type == 1 || type == 2)
        n = kChineseNumber;
    
    fprintf(stdout, "-------------------------------------------------------\n");
    fprintf(stdout, "start test\n");

    int correct[n] = {0}, total[n] = {0};
    for (int i = 0; i < test_labels.size(); i++)
    {
        int result = identify(test_images[i]);
        if (result == test_labels[i])
        {
            correct[test_labels[i]]++;
            correct_sum++;
        }
        else
        {
            fprintf(stdout,"[%d][%d]", test_labels[i], result);
            if (type == 1 || type == 2)
                fprintf(stdout,"[error]:correct result:%s\t result:%s\n", kChars[test_labels[i] + kCharsNumber], kChars[result + kCharsNumber]);
            else     
                fprintf(stdout,"[error]:correct result:%s\t result:%s\n", kChars[test_labels[i]], kChars[result]);
            
        }
        total[test_labels[i]]++;
        total_sum++;
    }
    for (int i = 0; i < n; i++)
    {
        total[i] == 0 ? 1 : total[i];
        float rate = float(correct[i]) / total[i];
        if(type == 1 || type == 2)
            fprintf(stdout, "[%s]total:%d\tcorrect:%d\trate:%.2f\n", kChars[kCharsNumber + i], total[i], correct[i], rate);
        else
            fprintf(stdout, "[%s]total:%d\tcorrect:%d\trate:%.2f\n", kChars[i], total[i], correct[i], rate);
        fprintf(stdout, "-------------------------------------------------------\n");
    }

    total_sum == 0 ? 1 : total_sum;
    float rate_sum = float(correct_sum) / total_sum;
    fprintf(stdout, "total_sum:%d\tcorrect_sum:%d\trate_sum:%.2f\n", total_sum, correct_sum, rate_sum);
    fprintf(stdout, "end test\n");
}


SVMTrainer::SVMTrainer()
{
    svm_ptr = SVM::create();
}

Ptr<TrainData> SVMTrainer::preprocessTrainData()
{
    Mat train_images;
    vector<int> train_labels;

    vector<string> is_plate_train_files;
    lsDir(kSVMIsPlateTrainSamplePath,is_plate_train_files);
    random_shuffle(is_plate_train_files.begin(), is_plate_train_files.end());

    vector<string> is_plate_test_files;
    lsDir(kSVMIsPlateTestSamplePath,is_plate_test_files);
    random_shuffle(is_plate_test_files.begin(), is_plate_test_files.end());

    vector<string> no_plate_train_files;
    lsDir(kSVMNoPlateTrainSamplePath,no_plate_train_files);
    random_shuffle(no_plate_train_files.begin(), no_plate_train_files.end());

    vector<string> no_plate_test_files;
    lsDir(kSVMNoPlateTestSamplePath,no_plate_test_files);
    random_shuffle(no_plate_test_files.begin(), no_plate_test_files.end());
       

    fprintf(stdout,"%s\n",kSVMIsPlateTrainSamplePath);
    fprintf(stdout,"%s\n",kSVMIsPlateTestSamplePath);
    fprintf(stdout,"%s\n",kSVMNoPlateTrainSamplePath);
    fprintf(stdout,"%s\n",kSVMNoPlateTestSamplePath);

    for(int i = 0; i < is_plate_train_files.size(); i++)
    {
        Mat input_mat = imread(is_plate_train_files[i],IMREAD_COLOR);

        if(input_mat.empty())
            continue;

        Mat feature_mat = svmFeatures(input_mat);
        train_images.push_back(feature_mat);
        train_labels.push_back(1);
    }  
    for(int i = 0; i < is_plate_test_files.size(); i++)
    {
        Mat input_mat = imread(is_plate_test_files[i],IMREAD_COLOR );

        if(input_mat.empty())
            continue;

        test_images.push_back(input_mat);
        test_labels.push_back(1);
    }
    for(int i = 0; i < no_plate_train_files.size(); i++)
    {
        Mat input_mat = imread(no_plate_train_files[i],IMREAD_COLOR );

        if(input_mat.empty())
            continue;

        Mat feature_mat = svmFeatures(input_mat);
        train_images.push_back(feature_mat);
        train_labels.push_back(0);
    }
    for(int i = 0; i < no_plate_test_files.size(); i++)
    { 
        Mat input_mat = imread(no_plate_test_files[i],IMREAD_COLOR );

        if(input_mat.empty())
            continue;

        test_images.push_back(input_mat);
        test_labels.push_back(0);
    }

    train_images.convertTo(train_images,CV_32FC1);
    return TrainData::create(train_images,ROW_SAMPLE,train_labels);
}

bool SVMTrainer::judge(const Mat &in)
{
    Mat feature_mat = svmFeatures(in);
    int result = int(svm_ptr->predict(feature_mat));
    if(result == 0)
        return false;
    else 
        return true;
}

void SVMTrainer::train()
{
    svm_ptr->setType(SVM::C_SVC);
    svm_ptr->setKernel(SVM::RBF);
    svm_ptr->setDegree(0.1);
    svm_ptr->setGamma(0.1);
    svm_ptr->setCoef0(0.1);
    svm_ptr->setC(1);
    svm_ptr->setNu(0.1);
    svm_ptr->setP(0.1);
    svm_ptr->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER, 20000, 0.0001));

    Ptr<TrainData> train_data = preprocessTrainData();

    fprintf(stdout, "++++++++++++++++++++++++++++++++++++++++\n");
    fprintf(stdout, "start train\n");
    svm_ptr->trainAuto(train_data, 10, SVM::getDefaultGrid(SVM::C),
    SVM::getDefaultGrid(SVM::GAMMA), SVM::getDefaultGrid(SVM::P),
    SVM::getDefaultGrid(SVM::NU), SVM::getDefaultGrid(SVM::COEF),
    SVM::getDefaultGrid(SVM::DEGREE), true);
    fprintf(stdout, "end train\n");
    fprintf(stdout, "++++++++++++++++++++++++++++++++++++++++\n");

    svm_ptr->save(kSVMModelPath);
}

void SVMTrainer::test()
{
    int total = 0,ptrue_rfalse = 0,pfalse_rtrue = 0,correct = 0;
    float rate = 0.f;

   
    fprintf(stdout, "-------------------------------------------------------\n");
    fprintf(stdout, "start test\n");

    for(int i = 0; i < test_labels.size(); i++)
    {
        bool result = judge(test_images[i]);
        if(result != bool(test_labels[i]))
        {
            if(result == false && test_labels[i] == 1)
                pfalse_rtrue++;
            else if(result == true && test_labels[i] == 0)
                ptrue_rfalse++;
        }
        else
            correct++;
        total++;
    }

    total = (total == 0) ? 1 : total;
    rate = float(correct) / total;
    fprintf(stdout,"total:%d\ncorrect:%d\nrate:%2f\npfalse_rtrue:%d\nptrue_rfalse:%d\n",
            total,
            correct,
            rate,
            pfalse_rtrue,
            ptrue_rfalse
    );

    fprintf(stdout, "end test\n");
    fprintf(stdout, "-------------------------------------------------------\n");
}