#include "track.h"
using namespace engine;
using namespace std;

// delegate to the constructor taking lines
track::track()
: track({}, {0, 0}, "")
{}

track::track(vector<line> lines_, vec2d start_pos, string name)
: lines(std::move(lines_)), start_pos(start_pos), name(name)
{
    line_counts[LT_NORMAL] = line_counts[LT_ACCELERATION] = line_counts[LT_SCENERY] = 0;
    
    for (auto& l : lines) {
        register_line(l);
    }
}

const line& track::get_line(line_id name) const
{
    return *get_line_ptr(name);
}

const std::vector<line>& track::get_lines() const
{
    return lines;
}

const std::string& track::get_name() const
{
    return name;
}

// templated so we can get a const and non-const version
template <typename T>
T* find_line(T* begin, T* end, line_id name, bool return_closest)
{
    if (begin == end) {
        // empty range, won't find a thing here
        
        if (return_closest) {
            return begin;
        } else {
            return nullptr;
        }
    }
    
    T* mid = begin + (end - begin) / 2;
    
    if (mid->name == name) {
        return mid;
    } else if (mid->name < name) {
        // the line will be in the upper half
        return find_line(mid + 1, end, name, return_closest);
    } else {
        // the line will be in the lower half
        return find_line(begin, mid, name, return_closest);
    }
}


line_id track::add_new_line(line l)
{
    if (lines.size() == 0) {
        l.name = 0;
    } else {
        l.name = lines.back().name + 1;
    }
    
    lines.push_back(l);
    register_line(l);
    return l.name;
}

line_id track::insert_line(line l)
{
    auto* existing = find_line(lines.data(), lines.data() + lines.size(), l.name, true);
    
    if (existing) {
        size_t offset = existing - lines.data();
        
        if (existing->name == l.name) {
            // already have a line with that name
            throw runtime_error("line already exists with that name");
        } else if (existing->name < l.name) {
            // we need to insert after the existing line
            lines.insert(lines.begin() + offset + 1, l);
        } else {
            // insert before the existing line
            lines.insert(lines.begin() + offset, l);
        }
    } else {
        lines.push_back(l);
    }
    
    register_line(l);
    return l.name;
}

void track::mark(line_id lid)
{
    auto* line = get_line_ptr(lid);
    line->marked = true;
    deregister_line(*line);
}

void track::sweep()
{
    size_t offset = 0;
    
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].marked) {
            ++offset;
        } else {
            lines[i - offset] = lines[i];
        }
    }
    
    lines.resize(lines.size() - offset);
}

int track::line_count() const
{
    return line_count(LT_NORMAL) + line_count(LT_ACCELERATION) + line_count(LT_SCENERY);
}

int track::line_count(line_type type) const
{
    return line_counts[type];
}

void track::set_start_pos(vec2d new_start_pos)
{
    start_pos = new_start_pos;
}

vec2d track::get_start_pos() const
{
    return start_pos;
}

void track::set_name(std::string new_name)
{
    name = new_name;
}

line* track::get_line_ptr(line_id name)
{
    return find_line(lines.data(), lines.data() + lines.size(), name, false);
}

const line* track::get_line_ptr(line_id name) const
{
    return find_line(lines.data(), lines.data() + lines.size(), name, false);
}

void track::register_line(const line& line)
{
    ++line_counts[line.type];
}

void track::deregister_line(const line& line)
{
    --line_counts[line.type];
}
