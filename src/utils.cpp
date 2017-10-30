#include "utils.h"


void lsDir(const string &dir_path,vector<string> &file_path_vec)
{
    list<string> subfolders_list;
    subfolders_list.push_back(dir_path);
    while(!subfolders_list.empty())
    {
        string curr_folder(subfolders_list.back());
        if(*(curr_folder.end() - 1) != '/')
            curr_folder.push_back('/');
        
        DIR *pdir = opendir(curr_folder.c_str());
        subfolders_list.pop_back();
        if(!pdir)
            continue;
        
        dirent *pdirent = NULL;
        while((pdirent = readdir(pdir)) != NULL)
        {   
            if((strcmp(pdirent->d_name,".") != 0) && (strcmp(pdirent->d_name,"..") != 0))
            {
                string file_path;
                file_path.append(curr_folder.c_str());
                file_path.append(pdirent->d_name);

                if(IS_DIR(file_path.c_str()))
                {
                    subfolders_list.push_back(file_path);
                }
                else
                {
                    file_path_vec.push_back(file_path);
                }
            }
        }
        closedir(pdir);
    }
    
}

void saveImage(const Mat &in,const string save_path,const string save_name,int number)
{
    string temp(save_name);
    int index = temp.find_last_of('.');
    if(index == string::npos)
        return;

    string suffix = temp.substr(index);
    temp.erase(index);

    ostringstream oss;
    oss << save_path << '/' << temp << '-' << number << suffix;

    imwrite(oss.str(),in);
}