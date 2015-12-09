#include "../platform.hpp"
#include "json_source.hpp"
#include "../util.h"
#include "json.h"
using namespace engine;
using namespace std;

static std::string track_type = "LineOnline Track";
static string short_track_type = "LRJS";

const std::string& json_source::get_track_type() const
{
    return track_type;
}

const std::string& json_source::get_short_track_type() const
{
    return short_track_type;
}

std::vector<engine::track> json_source::load_tracks(const std::string& path)
{
    try {
        std::ifstream f(path);
        
        Json::Reader reader;
        Json::Value root;
        
        if (!reader.parse(f , root)) {
            throw runtime_error("parse error");
        }
        
        if (root.get("version", "X").asString() != "6.2") {
            throw runtime_error("invalid version");
        }
        
        if (root["label"].isNull()) {
            throw runtime_error("no label");
        }
        
        vec2d start_pos = {0, 0};
        if (!root["startPosition"].isNull()) {
            start_pos = {root["startPosition"]["x"].asDouble(),
                root["startPosition"]["y"].asDouble()};
        }
        
        string label = root["label"].asString();
        
        vector<line> lines;
        for (auto& l : root["lines"]) {
            line line;
            line.name = l["id"].asInt();
            line.inverse = l["flipped"].asInt() != 0;
            line.marked = false;
            line.type = (line_type)l["type"].asInt();
            line.p0 = {l["x1"].asDouble(), l["y1"].asDouble()};
            line.p1 = {l["x2"].asDouble(), l["y2"].asDouble()};
            line.snap_mode = (uint8_t)l["extended"].asInt();
            line.p0_snap = l.get("leftLine", -1).asInt();
            line.p1_snap = l.get("rightLine", -1).asInt();
            
            lines.push_back(line);
        }
        
        return vector<track> { track(std::move(lines), start_pos, label) };
    } catch (...) {
        return vector<track> { };
    }
}
