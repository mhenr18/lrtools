#include "trk.h"
#include "trk_sink.h"
#include "util.h"
using namespace engine;
using namespace std;

static string track_type = "Line Rider: Advanced Track (Version 1)";
static string short_track_type = "LRAv1";
static string plural_track_type = "Line Rider: Advanced Tracks (Version 1)";

static void write_byte(uint8_t value, vector<uint8_t>& output)
{
    output.push_back(value);
}

static void write_int16(int16_t value, vector<uint8_t>& output)
{
    write_byte((uint8_t)(value & 0xFF), output);
    write_byte((uint8_t)((value >> 8) & 0xFF), output);
}

static void write_int32(int32_t value, vector<uint8_t>& output)
{
    write_byte((uint8_t)(value & 0xFF), output);
    write_byte((uint8_t)((value >> 8) & 0xFF), output);
    write_byte((uint8_t)((value >> 16) & 0xFF), output);
    write_byte((uint8_t)((value >> 24) & 0xFF), output);
}

static void write_uint64(uint64_t value, vector<uint8_t>& output)
{
    write_byte((uint8_t)(value & 0xFF), output);
    write_byte((uint8_t)((value >> 8) & 0xFF), output);
    write_byte((uint8_t)((value >> 16) & 0xFF), output);
    write_byte((uint8_t)((value >> 24) & 0xFF), output);
    write_byte((uint8_t)((value >> 32) & 0xFF), output);
    write_byte((uint8_t)((value >> 40) & 0xFF), output);
    write_byte((uint8_t)((value >> 48) & 0xFF), output);
    write_byte((uint8_t)((value >> 56) & 0xFF), output);
}

static void write_string(const string& str, vector<uint8_t>& output)
{
    for (auto& c : str) {
        write_byte((uint8_t)c, output);
    }
}

static void write_double(double value, vector<uint8_t>& output)
{
    // TODO: universalise this w.r.t endianness
    uint64_t raw;
    memcpy(&raw, &value, sizeof raw);
    
    write_uint64(raw, output);
}

static uint8_t type_to_lra_type(line_type type)
{
    switch (type) {
        case LT_NORMAL:
            return 1;
        case LT_ACCELERATION:
            return 2;
        case LT_SCENERY:
            return 0;
        default:
            throw runtime_error("invalid line type");
    }
}

bool trk_sink::available_for_tracks(const std::vector<engine::track>& tracks)
{
    // trk v1 is always available (at the moment)
    return true;
}

const std::string& trk_sink::get_track_type() const
{
    return track_type;
}

const std::string& trk_sink::get_short_track_type() const
{
    return short_track_type;
}

const std::string& trk_sink::get_track_type_plural() const
{
    return plural_track_type;
}

static vector<uint8_t> save_track(const track& t)
{
    vector<uint8_t> output;
    
    write_int32(TRK_MAGIC, output);
    write_byte(1, output);
    write_int16(0, output);
    //write_string("", output);
    
    write_double(t.get_start_pos().x, output);
    write_double(t.get_start_pos().y, output);
    write_int32(t.line_count(), output);
    
    for (auto& l : t.get_lines()) {
        uint8_t flags = type_to_lra_type(l.type) & 31;
        flags |= l.snap_mode << 5;
        flags |= (l.inverse ? 1 : 0) << 7;
        
        write_byte(flags, output);
        
        if (l.type != LT_SCENERY) {
            write_int32(l.name, output);
            
            if (l.snap_mode) {
                write_int32(l.p0_snap, output);
                write_int32(l.p1_snap, output);
            }
        }
        
        write_double(l.p0.x, output);
        write_double(l.p0.y, output);
        write_double(l.p1.x, output);
        write_double(l.p1.y, output);
    }
    
    return output;
}

void trk_sink::save_track(const engine::track& track, const std::string& path)
{
    auto saved = ::save_track(track);
    write_file(path, saved);
}

std::vector<std::string> trk_sink::save_tracks(const std::vector<engine::track>& tracks,
                                               const std::string& folder)
{
    vector<string> saved_paths;
    
    for (auto& t : tracks) {
        string path = folder + "/" + t.get_name() + ".trk";
        int i = 1;
        
        while (file_exists(path)) {
            path = folder + "/" + t.get_name() + " " + to_string(i) + ".trk";
            ++i;
        }
        
        save_track(t, path);
        saved_paths.push_back(move(path));
    }
    
    return saved_paths;
}
