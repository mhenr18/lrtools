#ifndef util_h
#define util_h

#include <cstdint>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "track.h"

class binary_reader {
public:
    binary_reader(const std::string& path);
    
    uint8_t read_byte();
    int16_t read_int16();
    int32_t read_int32();
    uint64_t read_uint64();
    std::string read_string(size_t length);
    double read_double();
    
private:
    std::ifstream f;
};

class strfmt {
public:
    template <typename T>
    strfmt& operator <<(T&& rhs)
    {
        ss << std::forward<T>(rhs);
        return *this;
    }
    
    operator std::string() const
    {
        return ss.str();
    }
    
private:
    std::stringstream ss;
};

std::string pre_pad(std::string s, int length);
std::string post_pad(std::string s, int length);
std::string limit_length(std::string s, int max_length);

std::vector<uint8_t> read_file(const std::string& path);
void write_file(const std::string& path, const std::vector<uint8_t> output);
bool file_exists(const std::string& path);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

struct track_specifier {
    std::string path, name;
    bool has_name;
};

track_specifier parse_specifier(const std::string& spec);
std::vector<engine::track> load_from_specifier(track_specifier specifier);

#endif
