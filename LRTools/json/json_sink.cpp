#include <fstream>
#include "json.h"
#include "json_sink.h"
#include "util.h"
using namespace engine;
using namespace std;

static string track_type = "LineOnline Track";
static string short_track_type = "LRJS";
static string plural_track_type = "LineOnline Tracks";

bool json_sink::available_for_tracks(const std::vector<engine::track>& tracks)
{
    // json is always available (at the moment)
    return true;
}

const std::string& json_sink::get_track_type() const
{
    return track_type;
}

const std::string& json_sink::get_short_track_type() const
{
    return short_track_type;
}

const std::string& json_sink::get_track_type_plural() const
{
    return plural_track_type;
}

void json_sink::save_track(const engine::track& t, const std::string& path)
{
    Json::Value root = Json::objectValue;
    root["label"] = t.get_name();
    root["version"] = "6.2";
    
    if (t.get_start_pos() != vec2d(0, 0)) {
        root["startPosition"] = Json::objectValue;
        root["startPosition"]["x"] = t.get_start_pos().x;
        root["startPosition"]["y"] = t.get_start_pos().y;
    }
    
    root["lines"] = Json::arrayValue;
    
    for (auto& l : t.get_lines()) {
        Json::Value line = Json::objectValue;
        line["id"] = l.name;
        line["type"] = (int)l.type;
        line["x1"] = l.p0.x;
        line["y1"] = l.p0.y;
        line["x2"] = l.p1.x;
        line["y2"] = l.p1.y;
        line["extended"] = (int)l.snap_mode;
        line["flipped"] = (l.inverse ? 1 : 0);
        
        if (l.p0_snap != -1) {
            line["leftLine"] = l.p0_snap;
        }
        
        if (l.p1_snap != -1) {
            line["rightLine"] = l.p1_snap;
        }
        
        root["lines"].append(line);
    }
    
    ofstream f(path);
    Json::StyledWriter writer;
    f << writer.write(root);
}

std::vector<std::string> json_sink::save_tracks(const std::vector<engine::track>& tracks,
                                               const std::string& folder)
{
    throw runtime_error("not implemented");
}
