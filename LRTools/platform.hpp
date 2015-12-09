#ifndef platform_hpp
#define platform_hpp

#include <functional>
#include <string>
#include <vector>

std::string expand_path(const std::string& path);
bool is_file(const std::string& path);
bool is_directory(const std::string& path);
std::vector<std::string> list_directory(const std::string& path);

std::vector<std::string> split_path(const std::string& path);
std::string strip_extension(const std::string& path);
std::string get_extension(const std::string& path);

#endif
