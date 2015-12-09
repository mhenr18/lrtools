#include "../platform.hpp"
#include "sol_source.hpp"
#include "util.h"
#include "amf0.h"
using namespace engine;
using namespace std;

static std::string track_type = "Beta 2 Track";
static string short_track_type = "Beta2";

const std::string& sol_source::get_track_type() const
{
    return track_type;
}

const std::string& sol_source::get_short_track_type() const
{
    return short_track_type;
}

std::vector<engine::track> sol_source::load_tracks(const std::string& path)
{
    try {
        vector<track> tracks;
        auto* sol = read_sol(path.c_str());
        
        if (!sol) {
            throw runtime_error("file is not a sol");
        }
        
        auto* track_list = sol->data.properties;
        
        if (!track_list || track_list->type != AMF0_ECMA_ARRAY) {
            throw runtime_error("file does not contain track array");
        }
        
        // find the track
        auto* curr = track_list->data.properties;
        while (curr) {
            auto* label = find_property("label", curr);
            if (!label) {
                curr = curr->next;
                continue;
            }
            
            auto* version = find_property("version", curr);
            auto* startLine = find_property("startLine", curr);
            
            //printf("found track: %s %s", label->data.string, version->data.string);
            
            if (string(version->data.string) != "6.2") {
                continue;
            }
            
            vec2d start_pos;
            start_pos.x = find_property("0", startLine)->data.number;
            start_pos.y = find_property("1", startLine)->data.number;
            
            vector<line> lines;
            
            // this is a track
            auto* data = find_property("data", curr);
            auto* line = data->data.properties;
            while (line) {
                double prevLine = find_property("6", line)->data.number;
                double nextLine = find_property("7", line)->data.number;
                
                engine::line l;
                l.p0.x = find_property("0", line)->data.number;
                l.p0.y = find_property("1", line)->data.number;
                l.p1.x = find_property("2", line)->data.number;
                l.p1.y = find_property("3", line)->data.number;
                l.snap_mode = (uint8_t)find_property("4", line)->data.number;
                l.inverse = (find_property("5", line)->data.number == 0 ? false : true);
                l.p0_snap = (isnan(prevLine) ? -1 : (line_id)prevLine);
                l.p1_snap = (isnan(nextLine) ? -1 : (line_id)nextLine);
                l.name = (line_id)find_property("8", line)->data.number;
                l.type = (line_type)find_property("9", line)->data.number;
                
                lines.push_back(l);
                
                line = line->next;
            }
            
            std::reverse(lines.begin(), lines.end());
            
            tracks.emplace_back(std::move(lines), start_pos, label->data.string);
            curr = curr->next;
        }
        
        return tracks;
    } catch (...) {
        return vector<track> {};
    }
}