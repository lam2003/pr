#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <list>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define IS_DIR(_path) ({struct stat st;\
        lstat(_path, &st);             \
        S_ISDIR(st.st_mode) != 0;})
void lsDir(const string &dir_path,vector<string> &file_path_vec);

void saveImage(const Mat &in,const string save_path,const string save_name,int number);
#endif 