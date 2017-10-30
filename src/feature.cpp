#include "feature.h"

Mat getProjectedMat(const Mat &in)
{
    assert(in.channels() == 1);
    const int VERTICAL = 0;
    const int HORIZONTAL = 1;

    Mat v_hist = projectedHistogram(in, VERTICAL);
    Mat h_hist = projectedHistogram(in, HORIZONTAL);

    int total_cols = v_hist.cols + h_hist.cols;

    Mat out = Mat::zeros(1, total_cols, CV_32FC1);
    int j = 0;
    for (int i = 0; i < v_hist.cols; i++)
    {
        out.at<float>(j) = v_hist.at<float>(i);
        j++;
    }
    for (int i = 0; i < h_hist.cols; i++)
    {
        out.at<float>(j) = h_hist.at<float>(i);
        j++;
    }
    return out;
}

Mat annFeatures(const Mat &in, int width, int height, int type)
{
    assert(in.channels() == 1);
    Mat out;
    if (type == 0)
    {
        Mat data_mat;
        Rect center_rect = getCenterRect(in);
        Mat center_mat = getRectMat(in, center_rect);

        resize(center_mat, data_mat, Size(width, height));
        Mat projected_mat = getProjectedMat(data_mat);

        int total_cols = projected_mat.cols + width * height;

        out = Mat::zeros(1, total_cols, CV_32F);
        int j = 0;
        for (int i = 0; i < projected_mat.cols; i++)
        {
            out.at<float>(j) = projected_mat.at<float>(i);
            j++;
        }
        for (int c = 0; c < data_mat.cols; c++)
        {
            for (int r = 0; r < data_mat.rows; r++)
            {
                out.at<float>(j) += float(data_mat.at<unsigned char>(c, r));
                j++;
            }
        }
    }
    else if (type == 1)
    {
        Mat data_mat;
        resize(in, data_mat, Size(width, height));

        float scale = 1.f / 255.f;
        data_mat.convertTo(data_mat, CV_32FC1, scale, 0);
        data_mat -= mean(data_mat);
        data_mat = data_mat.reshape(1, 1);

        Mat binary_mat;
        threshold(in, binary_mat, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        binary_mat = preprocessChar(binary_mat, width);
        Rect center_rect = getCenterRect(binary_mat);
        Mat center_mat = getRectMat(binary_mat, center_rect);

        Mat projected_mat = getProjectedMat(center_mat);
        projected_mat = projected_mat.reshape(1, 1);

        hconcat(data_mat, projected_mat, out);
    }
    return out;
}

Mat getColorProjectedMat(const Mat &in)
{
    assert(in.channels() == 3);

    Mat hsv_mat;
    cvtColor(in,hsv_mat,CV_BGR2HSV);
 
    int val[180] = {0};
 
    const uchar *col_data_ptr;
    for(int i = 0 ; i < hsv_mat.rows; i++)
    {
        col_data_ptr = hsv_mat.ptr<uchar>(i);
        for(int j = 0; j < hsv_mat.cols; j += 3)
        {
            int H = int(col_data_ptr[j]);
            if(H > (180 - 1)) 
                H = (180 - 1);
            else if(H < 0)
                H = 0;
            val[H]++;
        }
    }

    Mat hist_mat = Mat::zeros(1,180,CV_32F);

    for(int i = 0; i < hist_mat.cols; i++)
        hist_mat.at<float>(i) = float(val[i]);

    double min_val,max_val;
    minMaxLoc(hist_mat,&min_val,&max_val);

    if(max_val > 0)
        hist_mat.convertTo(hist_mat,-1,1.f / max_val,0);

    Mat out = hist_mat.clone();
    return out;
}

Mat svmFeatures(const Mat &in)
{
    assert(in.channels() == 3);

    Mat resize_mat;
    resize(in,resize_mat,Size(kSVMPlateWidth,kSVMPlateHeight));

    Mat gray_mat;
    cvtColor(resize_mat,gray_mat,CV_RGB2GRAY);
   
    Mat binary_mat;
    threshold(gray_mat,binary_mat,0,255,CV_THRESH_OTSU + CV_THRESH_BINARY);

    Mat projected_mat = getProjectedMat(binary_mat);
    projected_mat = projected_mat.reshape(1,1);

    Mat color_projected_mat = getColorProjectedMat(resize_mat);
    color_projected_mat = color_projected_mat.reshape(1,1);
   
    Mat out;
    hconcat(projected_mat,color_projected_mat,out);
    return out;
}