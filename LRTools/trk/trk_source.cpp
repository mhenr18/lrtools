#include <iostream>
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
    
    // non-empty feature strings indicate non-beta2 compliance
    if (feature_str != " " && feature_str != "" && feature_str != ";") {
        throw runtime_error("unsupported features");
    }
    
    vec2d start_pos(reader.read_double(), reader.read_double());
    int32_t num_lines = reader.read_int32();
    std::vector<line> lines;
    
    for (int32_t i = 0; i < num_lines; ++i) {
        uint8_t flags = reader.read_byte();
        uint8_t lra_type = (flags & 31);
        uint8_t snap_mode = (flags >> 5) & 3;
        
        line line;
        line.type = lra_type_to_type(lra_type);
        line.inverse = (flags >> 7) > 0;
        line.name = -1;
        line.snap_mode = snap_mode;
        line.marked = false;
        line.p0_snap = -1;
        line.p1_snap = -1;
        
        if (line.type != LT_SCENERY) {
            line.name = reader.read_int32();
            
            if (snap_mode) {
                line.p0_snap = reader.read_int32();
                line.p1_snap = reader.read_int32();
            }
        }
        
        line.p0 = {reader.read_double(), reader.read_double()};
        line.p1 = {reader.read_double(), reader.read_double()};
        
        lines.push_back(line);
    }
    
    // with everything loaded, we now need to deal with any out-of-order colliding lines
    // as LRA doesn't guarantee that lines are saved in monotonically increasing line id order
    int last_name = -99;
    for (int i = 0; i < lines.size(); ++i) {
        auto line = lines[i];
        
        if (line.type == LT_SCENERY) {
            continue;
        }
        
        if (line.name < last_name) {
            // scan back to find the correct insertion index
            // (ignore scenery lines in the reverse scan too)
            int j = i - 1;
            while (j >= 0 && (lines[j].type == LT_SCENERY || lines[j].name >= line.name)) {
                j--;
            }
            
            // remove line from lines
            lines.erase(lines.begin() + i);
            
            // now insert it at the given index
            lines.insert(lines.begin() + (j + 1), line);
            
            // now we want to come back to this index without having a different last_name
            --i;
            continue;
        }
        
        last_name = line.name;
    }
    
    // check to ensure the previous loop did its job
    // (this could probably be removed once I'm happy that the above algorithm is actually correct)
    last_name = -99;
    for (int i = 0; i < lines.size(); ++i) {
        auto line = lines[i];
        
        if (line.type != LT_SCENERY && line.name < last_name) {
            throw runtime_error(strfmt() << "line " << line.name << " is out of order");
        }
        
        if (line.type != LT_SCENERY) {
            last_name = line.name;
        }
    }
    
    // now iterate through and remap line names, because we expect to have a unique line
    // name for every single line, whereas LRA only does that for colliding lines
    std::unordered_map<line_id, line_id> id_remap;
    line_id next_name = 1;
    
    for (auto& l : lines) {
        line_id saved_name = l.name;
        l.name = next_name++;
        
        if (l.type != LT_SCENERY) {
            id_remap[saved_name] = l.name;
        }
    }
    
    for (auto& l : lines) {
        if (l.p0_snap != -1) {
            l.p0_snap = id_remap[l.p0_snap];
        } else {
            l.p0_snap = 0;
        }
        
        if (l.p1_snap != -1) {
            l.p1_snap = id_remap[l.p1_snap];
        } else {
            l.p1_snap = 0;
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
