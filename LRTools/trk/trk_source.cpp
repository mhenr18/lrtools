#include "../platform.hpp"
#include "trk.h"
#include "trk_source.h"
#include "util.h"
using namespace engine;
using namespace std;

static std::string track_type = "Line Rider: Advanced Track (Version 1)";
static string short_track_type = "LRAv1";

// this differs from the standard line type system so we need a conversion function.
static line_type lra_type_to_type(uint8_t lra_type)
{
    switch (lra_type) {
        case 0:
            return LT_SCENERY;
        case 1:
            return LT_NORMAL;
        case 2:
            return LT_ACCELERATION;
        default:
            throw runtime_error("invalid line type");
    }
}

const std::string& trk_source::get_track_type() const
{
    return track_type;
}

const std::string& trk_source::get_short_track_type() const
{
    return short_track_type;
}


static track load_track(binary_reader& reader)
{
    if (reader.read_int32() != TRK_MAGIC) {
        throw runtime_error("invalid magic");
    }
    
    if (reader.read_byte() != 1) {
        throw runtime_error("unsupported version");
    }
    
    int16_t feature_strlen = reader.read_int16();
    string feature_str = reader.read_string(feature_strlen);
    
    if (feature_str.length() != 1) {
        throw runtime_error("invalid feature string");
    }
    
    vec2d start_pos(reader.read_double(), reader.read_double());
    int32_t num_lines = reader.read_int32();
    std::vector<line> lines;
    
    // remap the saved id -> actual id, because LRA doesn't save the ids
    // of scenery lines even though we'd really like to keep them
    std::unordered_map<line_id, line_id> id_remap;
    line_id next_name = 1;
    
    for (int32_t i = 0; i < num_lines; ++i) {
        uint8_t flags = reader.read_byte();
        uint8_t lra_type = (flags & 31);
        uint8_t snap_mode = (flags >> 5) & 3;
        
        line line;
        line.type = lra_type_to_type(lra_type);
        line.inverse = (flags >> 7) > 0;
        line.name = next_name++;
        line.snap_mode = snap_mode;
        line.marked = false;
        line.p0_snap = 0;
        line.p1_snap = 0;
        
        if (line.type != LT_SCENERY) {
            line_id saved_name = reader.read_int32();
            id_remap[saved_name] = line.name;
            
            if (snap_mode) {
                // we don't re-name these now because we might not know about the line
                // that we're snapped to yet
                line.p0_snap = reader.read_int32();
                line.p1_snap = reader.read_int32();
            }
        }
        
        line.p0 = {reader.read_double(), reader.read_double()};
        line.p1 = {reader.read_double(), reader.read_double()};
        
        lines.push_back(line);
    }
    
    // with everything loaded, we now go through the lines and remap the saved snap ids
    for (auto& line : lines) {
        if (id_remap.count(line.p0_snap)) {
            line.p0_snap = id_remap[line.p0_snap];
        }
        
        if (id_remap.count(line.p1_snap)) {
            line.p1_snap = id_remap[line.p1_snap];
        }
    }
    
    // finally, we can return a track with these lines
    return track(std::move(lines), start_pos, "");
}

std::vector<engine::track> trk_source::load_tracks(const std::string& path)
{
    try {
        binary_reader r(path);
        track t = load_track(r);
        t.set_name(strip_extension(split_path(path).back()));
        
        return vector<track> { move(t) };
    } catch (...) {
        return vector<track> {};
    }
}
