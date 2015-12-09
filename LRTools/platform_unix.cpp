#include <cerrno>
#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wordexp.h>
#include "platform.hpp"
#include "util.h"
using namespace std;

string expand_path(const std::string& path)
{
    wordexp_t exp_result;
    wordexp(path.c_str(), &exp_result, 0);
    
    string expanded(exp_result.we_wordv[0]);
    wordfree(&exp_result);
    
    return expanded;
}

bool is_file(const string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    
    return statbuf.st_mode & S_IFREG;
}

bool is_directory(const string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    
    return statbuf.st_mode & S_IFDIR;
}

vector<string> list_directory(const string& path)
{
    vector<string> listing;
    DIR *dp;
    struct dirent *ep;
    
    dp = opendir(path.c_str());
    
    if (dp) {
        while ((ep = readdir(dp))) {
            auto p = path + "/" + string(ep->d_name);
            
            
            listing.push_back(p);
        }
        
        closedir(dp);
    } else {
        cerr << "unable to list directory `" << path << "': " << strerror(errno) << endl;
    }
    
    return listing;
}

std::vector<std::string> split_path(const std::string& path)
{
    return split(path, '/');
}

std::string strip_extension(const std::string& path)
{
    size_t lastindex = path.find_last_of(".");
    if (lastindex == std::string::npos) {
        return path;
    }
    
    return path.substr(0, lastindex);
}

std::string get_extension(const std::string& path)
{
    size_t lastindex = path.find_last_of(".");
    if (lastindex == std::string::npos) {
        return path;
    }
    
    return path.substr(lastindex + 1);
}
