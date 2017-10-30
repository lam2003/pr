#include "functions.h"

bool compareCPlateByRectTlX(const CPlate &cplate1,const CPlate &cplate2)
{
    return cplate1.getRect().tl().x < cplate2.getRect().tl().x;
}

bool compareCCharByScore(const CChar &cchar1, const CChar &cchar2)
{
    return cchar1.getScore() > cchar2.getScore();
}

bool compareCCharByCenterX(const CChar &cchar1,const CChar &cchar2)
{
    return cchar1.getCenterPoint().x < cchar2.getCenterPoint().x;
}

bool compareCCharByRectTlX(const CChar &cchar1,const CChar &cchar2)
{
    return cchar1.getRect().tl().x < cchar2.getRect().tl().x;
}

bool compareVec2iByX(const Vec2i &vec2i1,const Vec2i &vec2i2)
{
    return vec2i1[0] < vec2i2[0];
}

bool compareCCharByRectTlXGD(const CChar &cchar1,const CChar &cchar2)
{
    return cchar1.getRect().tl().x > cchar2.getRect().tl().x;
}

Mat getSyntheticMat(const Mat &in,int type)
{
    srand(unsigned(time(NULL)));
    int rand_type = rand() % 2;

    Mat out;
    float rand_num = -7.f;
    float rand_array[140];
    for (int i = 0; i < 140; i++)
    {
        rand_array[i] = rand_num;
        rand_num += 0.1f;
    }

    if ((type == 0 || type == 1) && rand_type == 0)
    {
        int x_offset = rand_array[rand() % 40 + 50];
        int y_offset = rand_array[rand() % 40 + 50];
        out = getTranslatedMat(in, x_offset, y_offset);
    }
    else if((type == 0 || type == 1) && rand_type == 1)
    {
        float angle = rand_array[rand() % 140];
        out = getRotatedMat(in,angle);
    }
    else if (type == 2)
    {
        int border_color = getBorderColor(in);
        float angle = rand_array[rand() % 100 + 20];
        out = getRotatedMat(in, angle, border_color);
    }
    return out;
}

int getBorderColor(const Mat &in)
{
    int sum = 0;
    for (int i = 0; i < in.rows; i++)
    {
        sum += in.at<uchar>(i, 0);
        sum += in.at<uchar>(i, in.cols - 1);
    }
    for (int i = 0; i < in.cols; i++)
    {
        sum += in.at<uchar>(0, i);
        sum += in.at<uchar>(in.rows - 1, i);
    }

    int out = sum / (in.cols + in.cols + in.rows + in.rows);
    return out;
}

bool verifyPlateSizes(const RotatedRect &rrect)
{
    float error = 0.65f;
    float aspect = 3.75f;
    float min_aspect = aspect - aspect * error;
    float max_aspect = aspect + aspect * error;
    float max_width = 600.f;
    float min_width = 30.f;
    float min_area = float(min_width * min_width / max_aspect);
    float max_area = float(max_width * max_width / min_aspect);
    float max_angle = 60.f;
    float min_angle = -60.f;

    float width = rrect.size.width;
    float height = rrect.size.height;
    float area = width * height;
    float ratio = width / height;
    float angle = rrect.angle;
    if (ratio < 1)
    {
        swap(width, height);
        ratio = width / height;
        angle = 90.f + angle;
    }

    if ((area < min_area || area > max_area) ||
        (ratio < min_aspect || ratio > max_aspect) ||
        (angle < min_angle || angle > max_angle) ||
        (width < min_width && width > max_width))
        return false;
    return true;
}


Rect getSafeRect(const Mat &in,const Point2f &center_point,float width,float height)
{
    int tlx = center_point.x - int(width / 2.f);
    int tly = center_point.y - int(height / 2.f);
    int brx = center_point.x + int(width / 2.f);
    int bry = center_point.y + int(height / 2.f);

    tlx = tlx > 0 ? tlx : 0;
    tly = tly > 0 ? tly : 0;
    brx = brx < in.cols ? brx : in.cols;
    bry = bry < in.rows ? bry : in.rows;

    Rect out;
    out.x = tlx;
    out.y = tly;
    out.width = brx - tlx;
    out.height = bry - tly;

    return out;
}

Rect_<float> getSafeRect(const Mat &in,const RotatedRect &rrect)
{
    Rect_<float> bound_rect = rrect.boundingRect();

    float tlx = bound_rect.x > 0 ? bound_rect.x : 0;
    float tly = bound_rect.y > 0 ? bound_rect.y : 0;
    float brx = bound_rect.x + bound_rect.width < in.cols ? tlx + bound_rect.width : in.cols;
    float bry = bound_rect.y + bound_rect.height < in.rows ? tly + bound_rect.height : in.rows;
  
    Rect_<float> out;
    out.x = tlx;
    out.y = tly;
    out.width = brx - tlx;
    out.height = bry - tly;
    
  /*   if(out.width <= 0 || out.height <= 0)
        out = Rect_<float>(); */
    
    return out;
}

Rect getCenterRect(const Mat &in)
{
    int top = 0;
    int bottom = in.rows - 1;
    for (int i = 0; i < in.rows; i++)
    {
        bool is_found = false;
        for (int j = 0; j < in.cols; j++)
        {
            if (in.data[i * in.step[0] + j] > 20)
            {
                top = i;
                is_found = true;
                break;
            }
        }

        if (is_found)
            break;
        
    }
    for (int i = in.rows - 1; i >= 0; i--)
    {
        bool is_found = false;
        for (int j = 0; j < in.cols; j++)
        {
            if (in.data[i * in.step[0] + j] > 20)
            {
                bottom = i;
                is_found = true;
                break;
            }
        }

        if (is_found)
            break;
    }

    int left = 0;
    int right = in.cols - 1;
    for (int j = 0; j < in.cols; j++)
    {
        bool is_found = false;
        for (int i = 0; i < in.rows; i++)
        {
            if (in.data[i * in.step[0] + j] > 20)
            {
                left = j;
                is_found = true;
                break;
            }
        }

        if (is_found)
            break;
    }
    for (int j = in.cols - 1; j >= 0; j--)
    {
        bool is_found = false;
        for (int i = 0; i < in.rows; i++)
        {
            if (in.data[i * in.step[0] + j] > 20)
            {
                right = j;
                is_found = true;
                break;
            }
        }
        if (is_found)
            break;
    }

    Rect out(Point(left, top), Point(right, bottom));
    return out;
}

Mat getRectMat(const Mat &in, const Rect &rect)
{
    Mat out = Mat::zeros(in.rows, in.cols, CV_8UC1);

    int x_offset = int(floor(float(in.cols - rect.width) / 2.f));
    int y_offset = int(floor(float(in.rows - rect.height) / 2.f));

    for (int i = 0; i < rect.height; i++)
    {
        for (int j = 0; j < rect.width; j++)
            out.data[out.step[0] * (i + y_offset) + j + x_offset] =
                in.data[out.step[0] * (i + rect.tl().y) + j + rect.tl().x];
    }
    return out;
}

float getOverThreshNum(const Mat &in, int thresh)
{
    float over_thresh_num = 0.f;
    if (in.rows > 1)
    {
        for (int i = 0; i < in.rows; i++)
        {
            if (in.data[in.step[0] * i] > thresh)
                over_thresh_num++;
        }
    }
    else if (in.cols > 1)
    {
        for (int i = 0; i < in.cols; i++)
        {
            if (in.data[i] > thresh)
                over_thresh_num++;
        }
    }
    return over_thresh_num;
}

Mat projectedHistogram(const Mat &in, int flag, int thresh)
{
    int cols = (flag) ? in.rows : in.cols;
    Mat out = Mat::zeros(1, cols, CV_32F);

    for (int i = 0; i < cols; i++)
    {
        Mat temp_mat = (flag) ? in.row(i) : in.col(i);
        out.at<float>(i) = getOverThreshNum(temp_mat, thresh);
    }

    double min_val, max_val;
    minMaxLoc(out, &min_val, &max_val);

    if (max_val > 0)
        out.convertTo(out, -1, 1.f / max_val, 0);

    return out;
}

Mat getTranslatedMat(const Mat &in, float x_offset, float y_offset, int bg_color)
{
    Mat translate_mat = Mat::eye(2, 3, CV_32F);
    translate_mat.at<float>(0, 2) = x_offset;
    translate_mat.at<float>(1, 2) = y_offset;

    Mat out;
    warpAffine(in, out, translate_mat, in.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(bg_color));
    return out;
}

Mat getRotatedMat(const Mat &in, float angle, int bg_color)
{
    Point2f center_point(in.cols / 2.f, in.rows / 2.f);
    Mat rotate_mat = getRotationMatrix2D(center_point, angle, 1.0);
    Mat out;
    warpAffine(in, out, rotate_mat, in.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(bg_color));
    return out;
}

Mat scaleImage(const Mat &in, const Size &size,float &scale_ratio)
{
    Mat out;
    if (in.cols > size.width || in.rows > size.height)
    {
        float width_ratio = float(in.cols / size.width);
        float height_ratio = float(in.rows / size.height);

        scale_ratio = width_ratio > height_ratio ? width_ratio : height_ratio;
        int new_width = in.cols / scale_ratio;
        int new_height = in.rows / scale_ratio;
        resize(in, out, Size(new_width, new_height));
    }
    else
        out = in.clone();

    return out;
}

bool verifyCharSizes(const Rect &rect)
{
    float error = 0.35f;
    float aspect = 45.f / 90.f;
    float min_aspect = 0.05f;
    float max_aspect = aspect + error * aspect;
    float ratio = (float)rect.width / (float)rect.height;

    if (ratio > min_aspect && ratio < max_aspect)
        return true;
    return false;
}

Mat paintImageByPoints(const vector<Point> &point_vec, const Rect &rect)
{
    int width_expend = 0;
    int height_expend = 0;

    if (rect.width > rect.height)
        height_expend = (rect.width - rect.height) / 2;
    else
        width_expend = (rect.height - rect.width) / 2;

    Mat out(rect.height + height_expend * 2, rect.width + width_expend * 2, CV_8UC1, Scalar(0, 0, 0));
    for (int i = 0; i < point_vec.size(); i++)
    {
        Point point = point_vec[i];
        int x = point.x - rect.tl().x + width_expend;
        int y = point.y - rect.tl().y + height_expend;
        if (x >= 0 && x < out.cols && y >= 0 && y < out.rows)
            out.data[out.step[0] * y + x] = Scalar(255,255,255).val[0];
    }
    return out;
}

Mat preprocessChar(const Mat &in, int char_size)
{
    int m = max(in.cols, in.rows);
    float x_offset = float(m / 2 - in.cols / 2);
    int y_offset = float(m / 2 - in.rows / 2);

    Mat out = getTranslatedMat(in, x_offset, y_offset);
    resize(out, out, Size(char_size, char_size));

    return out;
}

Rect adaptiveCharRect(const Rect &rect, int max_width)
{
    int width_expend = 0;
    if (rect.height > 3 * rect.width)
        width_expend = int(int(rect.height * 0.5f) - rect.width) * 0.5f;
    
    int tlx = rect.tl().x - width_expend > 0 ? rect.tl().x - width_expend : 0;
    int brx = rect.br().x + width_expend < max_width ? rect.br().x + width_expend : max_width;

    Rect out(tlx, rect.tl().y, brx - tlx, rect.height);
    return out;
}

bool judgeMserCharDiffRatio(const Mat &in, const Rect &rect, float thresh)
{
    Mat binary_mat;
    threshold(in(rect), binary_mat, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    Rect adaptive_rect = adaptiveCharRect(rect, in.cols);
    Mat adaptive_mat = in(adaptive_rect);

    Mat adaptive_binary_mat;
    threshold(adaptive_mat, adaptive_binary_mat, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    int diff = abs(countNonZero(adaptive_binary_mat) - countNonZero(binary_mat));
    float diff_ratio = float(diff) / float(rect.area());

    if (diff_ratio > thresh)
        return false;

    return true;
}

void notMaxSuppression(vector<CChar> &cchar_vec, float overlap)
{
    sort(cchar_vec.begin(), cchar_vec.end(), compareCCharByScore);

    vector<CChar>::iterator it = cchar_vec.begin();
    for (; it != cchar_vec.end(); it++)
    {
        CChar cchar1 = *it;
        Rect cchar_rect1 = cchar1.getRect();
        vector<CChar>::iterator itn = it + 1;

        for (; itn != cchar_vec.end();)
        {
            CChar cchar2 = *itn;
            Rect cchar_rect2 = cchar2.getRect();
            float iou = calcInsertOverUnion(cchar_rect1, cchar_rect2);
            if (iou > overlap)
                itn = cchar_vec.erase(itn);
            else
                itn++;
        }
    }
}

void notMaxSuppression(vector<CPlate> &cplate_vec, float overlap)
{
    sort(cplate_vec.begin(), cplate_vec.end(), compareCPlateByRectTlX);

    vector<CPlate>::iterator it = cplate_vec.begin();
    for (; it != cplate_vec.end(); it++)
    {
        CPlate cplate1 = *it;
        Rect cplate_rect1 = cplate1.getRect();
        vector<CPlate>::iterator itn = it + 1;

        for (; itn != cplate_vec.end();)
        {
            CPlate cplate2 = *itn;
            Rect cplate_rect2 = cplate2.getRect();
            float iou = calcInsertOverUnion(cplate_rect1, cplate_rect2);
            if (iou > overlap)
                itn = cplate_vec.erase(itn);
            else
                itn++;
        }
    }
}

Rect interRect(const Rect &rect1, const Rect &rect2)
{
    int x = rect1.x > rect2.x ? rect1.x : rect2.x;
    int y = rect1.y > rect2.y ? rect1.y : rect2.y;
    int width = (rect1.x + rect1.width < rect2.x + rect2.width ? rect1.x + rect1.width : rect2.x + rect2.width) - x;
    int height = (rect1.y + rect1.height < rect2.y + rect2.height ? rect1.y + rect1.height : rect2.y + rect2.height) - y;

    Rect out(x, y, width, height);
    if (out.width <= 0 || out.height <= 0)
        out = Rect();
    return out;
}

Rect mergeRect(const Rect &rect1, const Rect &rect2)
{
    int x = rect1.x < rect2.x ? rect1.x : rect2.x;
    int y = rect1.y < rect2.y ? rect1.y : rect2.y;
    int width = (rect1.x + rect1.width > rect2.x + rect2.width ? rect1.x + rect1.width : rect2.x + rect2.width) - x;
    int height = (rect1.y + rect1.height > rect2.y + rect2.height ? rect1.y + rect1.height : rect2.y + rect2.height) - y;

    Rect out(x, y, width, height);
    return out;
}

float calcInsertOverUnion(const Rect &rect1, const Rect &rect2)
{
    Rect inter_rect = interRect(rect1, rect2);
    Rect merge_rect = mergeRect(rect1, rect2);

    float iou = float(inter_rect.area()) / float(merge_rect.area());

    return iou;
}

bool isCharsBelongToOneGroup(const CChar &cchar1, const CChar &cchar2)
{
    Rect rect1 = cchar1.getRect();
    Rect rect2 = cchar2.getRect();

    //候选字符具有相似的矩型高度
    float height1 = float(rect1.height);
    float height2 = float(rect2.height);

    float height_diff = abs(height1 - height2);
    float height_diff_ratio = height_diff / min(height1, height2);

    if (height_diff_ratio > 0.25f)
        return false;

    float center_y1 = float(rect1.tl().y + rect1.height / 2);
    float center_y2 = float(rect2.tl().y + rect2.height / 2);
    float center_y_diff = abs(center_y1 - center_y2);
    float center_y_diff_ratio = center_y_diff / min(height1,height2);

    if(center_y_diff_ratio > 0.25f)
        return false;

    //候选字符在同一水平线上,但是中心点不那么靠近
    float center_x1 = float(rect1.tl().x + rect1.width / 2);
    float center_x2 = float(rect2.tl().x + rect2.width / 2);
    float center_x_diff = abs(center_x1 - center_x2);
    float center_x_diff_ratio = center_x_diff / min(height1, height2);

    if (center_x_diff_ratio < 0.25f)
        return false;

    //候选字符在同一水平线上,距离不能太远
    float min_brx = float(min(rect1.br().x, rect2.br().x));
    float max_tlx = float(max(rect1.tl().x, rect2.tl().x));

    float x_diff = abs(max_tlx - min_brx);
    float x_diff_ratio = x_diff / min(height1, height2);

    if (x_diff_ratio > 1.f)
        return false;

    return true;
}

void mergeCharToGroup(const vector<CChar> &cchar_vec, vector<vector<CChar> > &cchar_group_vec)
{
    vector<int> group_label_vec;

    int group_num;

    if (cchar_vec.size() < 0)
        return;

    group_num = partition(cchar_vec, group_label_vec, &isCharsBelongToOneGroup);

    for (int i = 0; i < group_num; i++)
    {
        vector<CChar> cchar_group;

        for (int j = 0; j < cchar_vec.size(); j++)
        {
            int group_label = group_label_vec[j];

            if (group_label == i)
                cchar_group.push_back(cchar_vec[j]);
        }

        if (cchar_group.size() < 2)
            continue;

        cchar_group_vec.push_back(cchar_group);
    }
}

void removeRightOutliers(vector<CChar> &cchar_vec,vector<CChar> &out_cchar_vec,float min_thresh,float max_thresh)
{
    sort(cchar_vec.begin(),cchar_vec.end(),compareCCharByCenterX);
    
    vector<float> k_vec;
   
    for(int i = 0; i + 1 < cchar_vec.size(); i++)
    {
        Vec4f line_vec4f;
        CChar cchar1 = cchar_vec.at(i);
        CChar cchar2 = cchar_vec.at(i + 1);

        vector<Point> point_vec;
        point_vec.push_back(cchar1.getCenterPoint());
        point_vec.push_back(cchar2.getCenterPoint());

        fitLine(Mat(point_vec),line_vec4f,CV_DIST_L2,0,0.01,0.01);
        
        float k = line_vec4f[1] / line_vec4f[0];

        k_vec.push_back(k);
    }

    int inlier_num = 0;
    int outlier_index = -1;

    for(int i = 0; i + 1 < k_vec.size(); i++)
    {
        float k1 = k_vec.at(i);
        float k2 = k_vec.at(i + 1);
        float k_diff = abs(k1 - k2);

        
        if(k_diff <= min_thresh)
            inlier_num++;
           
        if(inlier_num >= 2 && k_diff >= max_thresh)
        {
            
            outlier_index = i + 2;
            break;
        }
     
    }
   
    for(int i = 0; i < cchar_vec.size(); i++)
    {
       
        if(i != outlier_index)
        {
            CChar cchar = cchar_vec.at(i);
            out_cchar_vec.push_back(cchar);
        }
    }
   
   
}

void removeContainChar(vector<CChar> &cchar_vec,float thresh)
{
    sort(cchar_vec.begin(),cchar_vec.end(),compareCCharByRectTlX);
    
    vector<CChar>::iterator it = cchar_vec.begin();

    for(;it != cchar_vec.end();it++)
    {
        CChar cchar1 = *it;
        Rect cchar_rect1 = cchar1.getRect();
        vector<CChar>::iterator itn = it + 1;

        for(;itn != cchar_vec.end();)
        {
            CChar cchar2 = *itn;
            Rect cchar_rect2 = cchar2.getRect();

            Rect and_rect = cchar_rect1 & cchar_rect2;

            float thresh1 = 1.f - float(and_rect.area()) / float(cchar_rect1.area());
            float thresh2 = 1.f - float(and_rect.area()) / float(cchar_rect2.area());

            if(thresh1 < thresh || thresh2 < thresh)
            {
                it->setRect(it->getRect() | itn->getRect());
                itn = cchar_vec.erase(itn);
            }
            else 
                itn++;
        }
    }
}

void axesSearch(const Vec4f &cplate_line_vec4f,
                Point &cplate_left_point,
                Point &cplate_right_point,
                Rect &cplate_max_cchar_rect,
                Rect &cplate_rect,
                vector<CChar> &cchar_vec,
                vector<CChar> &out_cchar_vec,
                float thresh,
                int flag)
{

    vector<CChar> axes_cchar_vec;
    axes_cchar_vec.reserve(16);

    float k = cplate_line_vec4f[1] / cplate_line_vec4f[0];
    float x = cplate_line_vec4f[2];
    float y = cplate_line_vec4f[3];

    for (int i = 0; i < cchar_vec.size(); i++)
    {
        CChar cchar = cchar_vec.at(i);
        Rect cchar_rect = cchar.getRect();

        if (flag == 0)
        {
            if (cchar_rect.tl().x + cchar_rect.width > cplate_left_point.x)
                continue;
        }
        else if (flag == 1)
        {
            if (cchar_rect.tl().x < cplate_right_point.x)
                continue;
        }

        Point cchar_center_point = cchar.getCenterPoint();
        float cchar_x = float(cchar_center_point.x);
        float cchar_y = float(cchar_center_point.y);
        float guess_cchar_y = k * (cchar_x - x) + y;
        float y_diff_ratio = abs(guess_cchar_y - cchar_y) / cplate_max_cchar_rect.height;

      

        if(flag == 1 && cchar_rect.area() < int(cplate_max_cchar_rect.area() * 0.4f) && y_diff_ratio > 0.15f)
            continue;    
       
        /* float area_diff_ratio = float(abs(cchar_rect.area() - cplate_max_cchar_rect.area())) / float(cplate_max_cchar_rect.area());   
        if(area_diff_ratio > 0.3f && y_diff_ratio > 0.1f)
            continue;  */

        if (y_diff_ratio < thresh)
        {
            float width1 = float(cplate_max_cchar_rect.width);
            float height1 = float(cplate_max_cchar_rect.height);

            float width2 = float(cchar_rect.width);
            float height2 = float(cchar_rect.height);

            float height_diff_ratio = abs(height1 - height2) / height1;
            float width_diff_ratio = abs(width1 - width2) / width1;

            float area_diff_ratio = (cchar_rect.area() - cplate_max_cchar_rect.area()) / cplate_max_cchar_rect.area();
            if ((height_diff_ratio < 0.6f && width_diff_ratio < 0.8f && area_diff_ratio < 0.5f) || (cchar_rect.area() < cplate_max_cchar_rect.area() && cchar_rect.area() > cplate_max_cchar_rect.area() * 0.5))
                axes_cchar_vec.push_back(cchar);
        }
    }

    if (axes_cchar_vec.size() != 0)
    {
        removeContainChar(axes_cchar_vec,0.5);
        if (flag == 0)
            sort(axes_cchar_vec.begin(), axes_cchar_vec.end(), compareCCharByRectTlXGD);

        else if (flag == 1)
            sort(axes_cchar_vec.begin(), axes_cchar_vec.end(), compareCCharByRectTlX);

        CChar first_axes_cchar = axes_cchar_vec.at(0);
        
        float min_brx;
        float max_tlx;

        if(flag == 0)
        {
            min_brx = float(min(first_axes_cchar.getRect().br().x, cplate_left_point.x + cplate_max_cchar_rect.width / 2));
            max_tlx = float(max(first_axes_cchar.getRect().tl().x, cplate_left_point.x - cplate_max_cchar_rect.width / 2));
        }
        else if(flag == 1)
        {
            min_brx = float(min(first_axes_cchar.getRect().br().x, cplate_right_point.x + cplate_max_cchar_rect.width / 2));
            max_tlx = float(max(first_axes_cchar.getRect().tl().x, cplate_right_point.x - cplate_max_cchar_rect.width / 2));
        }

        float x_diff = abs(max_tlx - min_brx);
        float x_diff_ratio = x_diff / float(cplate_max_cchar_rect.height);
     
        if (x_diff_ratio > 0.5f)
            return;
        

        out_cchar_vec.push_back(first_axes_cchar);
        cplate_rect |= first_axes_cchar.getRect();

        if (flag == 0)
            cplate_left_point = first_axes_cchar.getCenterPoint();
        else if (flag == 1)
            cplate_right_point = first_axes_cchar.getCenterPoint();

        
        for (int i = 0; i + 1 < axes_cchar_vec.size(); i++)
        {
            CChar axes_cchar1 = axes_cchar_vec.at(i);
            CChar axes_cchar2 = axes_cchar_vec.at(i + 1);
            Rect axes_cchar_rect1 = axes_cchar1.getRect();
            Rect axes_cchar_rect2 = axes_cchar2.getRect();

        
            float min_brx = float(min(axes_cchar_rect1.br().x, axes_cchar_rect2.br().x));
            float max_tlx = float(max(axes_cchar_rect1.tl().x, axes_cchar_rect2.tl().x));
            
            float x_diff = float(abs(max_tlx - min_brx));
            float x_diff_ratio = x_diff / float(cplate_max_cchar_rect.height);
            
            float height_diff = float(abs(axes_cchar_rect1.height - axes_cchar_rect1.height));
            float height_diff_ratio = height_diff / float(cplate_max_cchar_rect.height);

            float center_y_diff = float(abs(axes_cchar1.getCenterPoint().y - axes_cchar2.getCenterPoint().y));
            float center_y_diff_ratio = center_y_diff / float(cplate_max_cchar_rect.height);
    
            if (x_diff_ratio > 0.6f || center_y_diff_ratio > 0.2f || height_diff_ratio > 0.2f)
                return;
            else
            {
                out_cchar_vec.push_back(axes_cchar2);
                cplate_rect |= axes_cchar_rect2;
                if (flag == 0)
                {
                    if (axes_cchar2.getCenterPoint().x < cplate_left_point.x)
                        cplate_left_point = axes_cchar2.getCenterPoint();
                }
                else if (flag == 1)
                {
                    if (axes_cchar2.getCenterPoint().x > cplate_right_point.x)
                        cplate_right_point = axes_cchar2.getCenterPoint();
                }
            }
        }
    }
}

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
                       int flag)
{

    float k = cplate_line_vec4f[1] / cplate_line_vec4f[0];
    float x = cplate_line_vec4f[2];
    float y = cplate_line_vec4f[3];
    int slide_length = int(window_ratio * cplate_max_cchar_rect.width);
    int slide_step  = 1;
    int from_x = 0;

  
   
    if(flag == 0)
        from_x = cplate_left_point.x - cplate_max_cchar_rect.width;
    
    else if(flag == 1)
        from_x = cplate_right_point.x + cplate_max_cchar_rect.width;
    

    vector<CChar> slide_cchar_vec;
    slide_cchar_vec.reserve(16);

    for(int slide_x = slide_length; slide_x > -slide_length; slide_x -= slide_step)
    {
        float temp_x = 0;
        if(flag == 0)
            temp_x = float(from_x - slide_x);

        else if(flag == 1)
            temp_x = float(from_x + slide_x);
        
        float temp_y = y + k * (temp_x - x); 

        Point slide_point(temp_x,temp_y);

        int chinese_height = int(1.1f * cplate_max_cchar_rect.height);
        int chinese_width = int(1.1f * cplate_max_cchar_rect.width);
        Rect rect;
        if(is_chinese)
            rect = Rect(Point2f(temp_x - chinese_width / 2,temp_y - chinese_height / 2),
                   Size(chinese_width, chinese_height));
        else 
            rect = Rect(Point2f(temp_x - cplate_max_cchar_rect.width / 2,temp_y - cplate_max_cchar_rect.height / 2),
                   Size(cplate_max_cchar_rect.width, cplate_max_cchar_rect.height));

        if(rect.tl().x < 0 || rect.tl().y < 0 || rect.br().x > in.cols || rect.br().y > in.rows)
            continue;
         
        Mat cchar_mat;
        threshold(in(rect),cchar_mat,cplate_otsu_level,255,CV_THRESH_BINARY);
        cchar_mat = preprocessChar(cchar_mat,kCharSize);

        CChar cchar;
        cchar.setMat(cchar_mat);
        cchar.setRect(rect);
        slide_cchar_vec.push_back(cchar);
    }

    if(is_chinese)
        CharIdentifier::getInstance()->classifyChinese(slide_cchar_vec);
    else
        CharIdentifier::getInstance()->classify(slide_cchar_vec);
     
    float overlap = 0.1;
    notMaxSuppression(slide_cchar_vec,overlap);

    for(int i = 0; i < slide_cchar_vec.size(); i++)
    {
        CChar cchar = slide_cchar_vec.at(i);
        Rect cchar_rect = cchar.getRect();
        Point cchar_center_point(cchar_rect.tl().x + cchar_rect.width / 2,cchar_rect.tl().y + cchar_rect.height / 2);
        if(cchar.getScore() > thresh && strcmp(cchar.getLabel().c_str(),"1") != 0)
        {
            cplate_rect |= cchar_rect;
            out_cchar_vec.push_back(cchar);
            if(flag == 0)
                cplate_left_point = cchar_center_point;
            else if(flag == 1)
                cplate_right_point = cchar_center_point;
        }
    }
  
}

void combineRect(const Mat &in, vector<CChar> &cchar_vec,vector<CChar> &out_cchar_vec,const Vec2i &cplate_dest_vec2i,const Rect &cplate_max_cchar_rect, float min_thresh, float max_thresh)
{
    if (cchar_vec.size() == 0)
        return;

    sort(cchar_vec.begin(), cchar_vec.end(), compareCCharByCenterX);
    int avg_dist = cplate_dest_vec2i[0] * cplate_dest_vec2i[0] + cplate_dest_vec2i[1] * cplate_dest_vec2i[1];

    vector<CChar> combine_cchar_vec;
    combine_cchar_vec.reserve(16);

    int i = 0;
    for (;i + 1 < cchar_vec.size(); i++)
    {
        CChar cchar1 = cchar_vec.at(i);
        CChar cchar2 = cchar_vec.at(i + 1);

        Point cchar_center_point1 = cchar1.getCenterPoint();
        Point cchar_center_point2 = cchar2.getCenterPoint();

        int x_diff = cchar_center_point1.x - cchar_center_point2.x;
        int y_diff = cchar_center_point1.y - cchar_center_point2.y;

        int dist = x_diff * x_diff + y_diff * y_diff;

        float ratio = float(dist) / float(avg_dist);

        Rect and_rect = cchar1.getRect() & cchar2.getRect();
        float thresh1 = 1.f - float(and_rect.area()) / float(cchar1.getRect().area());
        float thresh2 = 1.f - float(and_rect.area()) / float(cchar2.getRect().area());

        if (ratio > max_thresh)
        {
            float x_avg = float(cchar_center_point1.x + cchar_center_point2.x) / 2.f;
            float y_avg = float(cchar_center_point1.y + cchar_center_point2.y) / 2.f;

            float width = float(cplate_max_cchar_rect.width);
            float height = float(cplate_max_cchar_rect.height);

            Rect cchar_rect = getSafeRect(in, Point2f(x_avg, y_avg), width, height);

            combine_cchar_vec.push_back(cchar1);

            CChar cchar;
            cchar.setCenterPoint(Point(int(x_avg), int(y_avg)));
            cchar.setRect(cchar_rect);
            combine_cchar_vec.push_back(cchar);
        }
        else if (ratio < min_thresh)
        {
            Rect or_rect = cchar1.getRect() | cchar2.getRect();

            int x_avg = or_rect.tl().x + or_rect.width / 2;
            int y_avg = or_rect.tl().y + or_rect.height / 2;

            CChar cchar;
            cchar.setCenterPoint(Point(x_avg, y_avg));
            cchar.setRect(or_rect);
            combine_cchar_vec.push_back(cchar);

            i++;
        }
        else
            combine_cchar_vec.push_back(cchar1);
    }

    if (i + 1 == cchar_vec.size())
        combine_cchar_vec.push_back(cchar_vec.at(i));
    

    out_cchar_vec = combine_cchar_vec;
}

Mat getRotatedMat(const Mat &in,const Size &size,const Point2f &center_point,float angle)
{
    Mat enlarge_mat(int(in.rows * 1.5f),int(in.cols * 1.5f),in.type());

    float tlx = enlarge_mat.cols / 2 - center_point.x > 0 ? enlarge_mat.cols / 2 - center_point.x : 0;
    float tly = enlarge_mat.rows / 2 - center_point.y > 0 ? enlarge_mat.rows / 2 - center_point.y : 0;
    float width = tlx + in.cols < enlarge_mat.cols ? in.cols : enlarge_mat.cols - tlx;
    float height = tly + in.rows < enlarge_mat.rows ? in.rows : enlarge_mat.rows - tly;

  /*   if (width != in.cols || height != in.rows) 
        return Mat(); */

    Mat roi_mat = enlarge_mat(Rect_<float>(tlx,tly,width,height));
    addWeighted(roi_mat,0,in,1,0,roi_mat);

    Point2f enlarge_center_point(enlarge_mat.cols / 2,enlarge_mat.rows / 2);
    Mat rot_mat = getRotationMatrix2D(enlarge_center_point,angle,1);

    Mat rotated_mat;
    warpAffine(enlarge_mat,rotated_mat,rot_mat,Size(enlarge_mat.cols,enlarge_mat.rows),CV_INTER_CUBIC);

    Mat out;
    getRectSubPix(rotated_mat,size,enlarge_center_point,out);

    return out;
}

Mat affine(const Mat &in,float k)
{
    assert(in.channels() == 3);

    float width = float(in.cols) - 1;
    float height = float(in.rows) - 1;
    float x_diff = height * k;
    Point2f src_points[3];
    Point2f dest_points[3];
    if(k > 0)
    {
        src_points[0] = Point2f(0,0);
        src_points[1] = Point2f(width - x_diff,0);
        src_points[2] = Point2f(x_diff,height);

        dest_points[0] = Point2f(x_diff / 2.f,0);
        dest_points[1] = Point2f(width - x_diff / 2.f,0);
        dest_points[2] = Point2f(x_diff / 2,height);
    }
    else
    {
        src_points[0] = Point2f(-x_diff,0);
        src_points[1] = Point2f(width,0);
        src_points[2] = Point2f(0,height);

        dest_points[0] = Point2f(-x_diff / 2.f,0);
        dest_points[1] = Point2f(width + x_diff / 2.f,0);
        dest_points[2] = Point2f(-x_diff / 2,height);
    }

    Mat warp_mat = getAffineTransform(src_points,dest_points);

    Mat out;
    out.create(int(height + 1),int(width + 1),CV_8UC3);

    if(in.rows > kSVMPlateHeight || in.cols > kSVMPlateWidth)
        warpAffine(in,out,warp_mat,out.size(),CV_INTER_AREA);
    else
        warpAffine(in,out,warp_mat,out.size(),CV_INTER_CUBIC);

    return out;
}
 

bool isDeflection(const Mat &in,float angle,float &k)
{
    assert(in.channels() == 1);

    int height_index[3];
    int dist_index[3];

    height_index[0] = in.rows / 4;
    height_index[1] = in.rows / 2;
    height_index[2] = in.rows / 4 * 3;
    
    for(int i = 0; i < 3; i++)
    {
        int dist = 0;
        const uchar *col_data_ptr = in.ptr<uchar>(height_index[i]);
        int j = 0;
        for(; j < in.cols && col_data_ptr[j] == 0; j++);
        
        dist_index[i] = j;
    }


    float max_dist = dist_index[0] > dist_index[2] ? dist_index[0] : dist_index[2];
    float min_dist = dist_index[0] < dist_index[2] ? dist_index[0] : dist_index[2];

    float dist_diff = max_dist - min_dist;

    if(max_dist - dist_index[1] > in.cols / 32 || dist_index[1] - min_dist > in.cols / 32)
    {
        float guess_k = float(tan(angle * float(CV_PI) / 180.f));
        float k1 = float(dist_index[2] - dist_index[0]) / float(height_index[1]);
        float k2 = float(dist_index[1] - dist_index[0]) / float(height_index[0]);
        float k3 = float(dist_index[2] - dist_index[1]) / float(height_index[0]);
    
        k = abs(k1 - guess_k) < abs(k2 - guess_k) ? k1 : k2;
        k = abs(k - guess_k) < abs(k3 - guess_k) ? k : k3;
        
        return true;
    }
    else 
        k = 0.f;
    return false;
}

void deskew(CPlate &cplate)
{
    RotatedRect cplate_rrect = cplate.getRRect();

    float cplate_rrect_ratio = float(cplate_rrect.size.width) / float(cplate_rrect.size.height);
    float cplate_rrect_angle = cplate_rrect.angle;
    Size cplate_rrect_size = cplate_rrect.size;
    Rect_<float> cplate_bound_rect = cplate.getBoundRect();
    Mat cplate_mask_mat = cplate.getMaskMat();
    Mat cplate_bound_mat = cplate.getBoundMat();
    float cplate_k = cplate.getLineVec4f()[1] / cplate.getLineVec4f()[0];


/*     if(cplate_mask_mat.empty() || cplate_bound_mat.empty())
    {
        cplate.setMat(Mat::zeros(kSVMPlateHeight,kSVMPlateWidth,CV_8UC3));
        return;
    } */
    if(cplate_rrect_ratio < 1.f)
    {
        cplate_rrect_angle += 90.f;
        swap(cplate_rrect_size.width,cplate_rrect_size.height);
    }

    Mat cplate_mat;

    Point2f ref_center_point = cplate_rrect.center - cplate_bound_rect.tl();

  //  if((cplate_rrect_angle - 5.f < 0.f && cplate_rrect_angle + 5.f > 0.f) || 90.f == cplate_rrect_angle || -90.f == cplate_rrect_angle)
  //      cplate_mat = cplate_bound_mat.clone();
  //  else
 //   {
       
        Mat rotated_mat = getRotatedMat(cplate_bound_mat,cplate_rrect_size,ref_center_point,cplate_rrect_angle);
        Mat mask_rotated_mat = getRotatedMat(cplate_mask_mat,cplate_rrect_size,ref_center_point,cplate_rrect_angle);

      /*   if(rotated_mat.empty() || mask_rotated_mat.empty())
        {
            imshow("ss",cplate_bound_mat);
            imshow("sss",cplate_mask_mat);
            waitKey(0);
            cplate.setMat(Mat::zeros(kSVMPlateHeight,kSVMPlateWidth,CV_8UC3));
            return;
        } */

/*         float k;
        if(isDeflection(mask_rotated_mat,cplate_rrect_angle,k)) */
            cplate_mat = affine(rotated_mat,cplate_k);
 /*        else
            cplate_mat = rotated_mat; */

      /*   if(cplate_mat.cols * 1.f / cplate_mat.rows > 2.3f && cplate_mat.cols * 1.f / cplate_mat.rows < 6.f)
        { */
        if (cplate_mat.cols >= kSVMPlateWidth || cplate_mat.rows >= kSVMPlateHeight)
            resize(cplate_mat, cplate_mat, Size(kSVMPlateWidth,kSVMPlateHeight), 0, 0, INTER_AREA);
        else
            resize(cplate_mat, cplate_mat, Size(kSVMPlateWidth,kSVMPlateHeight), 0, 0, INTER_CUBIC);
/*         }
        else
            cplate_mat = Mat::zeros(kSVMPlateHeight,kSVMPlateWidth,CV_8UC3); */
      
  //  }
/*     imshow("sss",cplate_mat);
    waitKey(0);
    printf("%d\n",cplate_mat.channels()); */
    cplate.setMat(cplate_mat);
}

void enlargeCCharRect(const Mat &in,CChar &cchar,const Vec4f &cplate_line_vec4f,const Rect &cplate_max_cchar_rect)
{
    float k = cplate_line_vec4f[1] / cplate_line_vec4f[0];
    float x = cplate_line_vec4f[2];
    float y = cplate_line_vec4f[3];

    Rect cchar_rect = cchar.getRect();

    float guess_y = y + (float(cchar.getCenterPoint().x) - x) * k;

    cchar.setCenterPoint(Point(cchar.getCenterPoint().x,int(guess_y)));

    Rect new_cchar_rect = getSafeRect(in, cchar.getCenterPoint(),cplate_max_cchar_rect.width * 1.1f,cplate_max_cchar_rect.height * 1.1f);

    cchar.setRect(new_cchar_rect);
    
}