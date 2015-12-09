#include <iterator>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "util.h"
#include "track_source.h"
using namespace std;

binary_reader::binary_reader(const string& path)
: f(path, ios::binary)
{}

uint8_t binary_reader::read_byte()
{
    if (!f.good()) {
        throw runtime_error("buffer is empty");
    }
    
    uint8_t byte;
    f.read((char *)&byte, 1);
    
    if (f.eof()) {
        throw runtime_error("buffer is empty");
    }
    
    return byte;
}

int16_t binary_reader::read_int16()
{
    int16_t b0 = read_byte();
    int16_t b1 = read_byte();
    
    return b0 | (b1 << 8);
}

int32_t binary_reader::read_int32()
{
    int32_t b0 = read_byte();
    int32_t b1 = read_byte();
    int32_t b2 = read_byte();
    int32_t b3 = read_byte();
    
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

uint64_t binary_reader::read_uint64()
{
    uint64_t b0 = read_byte();
    uint64_t b1 = read_byte();
    uint64_t b2 = read_byte();
    uint64_t b3 = read_byte();
    uint64_t b4 = read_byte();
    uint64_t b5 = read_byte();
    uint64_t b6 = read_byte();
    uint64_t b7 = read_byte();
    
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24) |
    (b4 << 32) | (b5 << 40) | (b6 << 48) | (b7 << 56);
}

string binary_reader::read_string(size_t length)
{
    string str;
    for (auto i = 0; i < length; ++i) {
        str.push_back((char)read_byte());
    }
    
    return str;
}

double binary_reader::read_double()
{
    // TODO: universalise this w.r.t endianness
    uint64_t raw = read_uint64();
    double ret;
    
    // the "traditional" way of doing this is to type-pun with a union.
    // this breaks aliasing rules (i.e we're in undefined behaviour land), so we memcpy
    // it instead and pray for endianness not to ruin us
    memcpy(&ret, &raw, sizeof raw);
    return ret;
}

string pre_pad(string s, int length)
{
    while (s.length() < length)
    {
        s.insert(s.begin(), ' ');
    }
    
    return s;
}

string post_pad(string s, int length)
{
    while (s.length() < length)
    {
        s.insert(s.end(), ' ');
    }
    
    return s;
}


std::string limit_length(std::string s, int max_length)
{
    if (s.length() > max_length) {
        s = s.substr(0, max_length - 5);
        s.append("(...)");
    }
    
    return s;
}

vector<uint8_t> read_file(const string& path)
{
    ifstream file(path, ios::binary);
    return vector<uint8_t>(istreambuf_iterator<char>(file),
                           istreambuf_iterator<char>());
}

void write_file(const std::string& path, const std::vector<uint8_t> output)
{
    ofstream file(path, ios::binary);
    file.write((const char *)output.data(), output.size());
}

bool file_exists(const string& path)
{
    std::ifstream f(path);
    return f.good();
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

track_specifier parse_specifier(const std::string& spec)
{
    size_t spec_split = spec.find(':');

    if (spec_split != std::string::npos) {
        return { spec.substr(0, spec_split), spec.substr(spec_split + 1), true };
    } else {
        return { spec, "", false };
    }
}

std::vector<engine::track> load_from_specifier(track_specifier specifier)
{
    auto sources = create_track_sources();
    
    for (auto& src : sources) {
        auto tracks = src->load_tracks(specifier.path);
        
        if (tracks.size() > 0) {
            if (specifier.has_name) {
                vector<engine::track> specced;
                
                for (auto& t : tracks) {
                    if (t.get_name().find(specifier.name) == 0) {
                        specced.push_back(t);
                    }
                }
                
                return specced;
            } else {
                return tracks;
            }
        }
    }
    
    throw runtime_error("no tracks found");
}
