#include <iostream>
#include "convert.hpp"
#include "platform.hpp"
#include "track_sink.h"
#include "util.h"
using namespace std;

static int convert_main(int argc, const char **argv)
{
    if (argc < 3) {
        cerr << "usage: lrtools convert <input_track> <output_track>" << endl;
        return 1;
    }
    
    auto src_spec = parse_specifier(argv[1]);
    auto dst_spec = parse_specifier(argv[2]);
    
    string dst_type;
    string dst_extension = get_extension(dst_spec.path);
    
    if (dst_extension == "trk") {
        dst_type = "LRAv1";
    } else if (dst_extension == "json") {
        dst_type = "LRJS";
    } else if (dst_extension == "sol") {
        dst_type = "Beta2";
    } else {
        throw runtime_error("unknown output track type");
    }
    
    try {
        auto src_tracks = load_from_specifier(src_spec);
        if (src_tracks.size() > 1) {
            throw runtime_error("unable to convert multiple tracks at once");
        }
        
        cout << src_tracks.front().get_name() << endl;
        
        if (dst_spec.has_name) {
            src_tracks.front().set_name(dst_spec.name);
        }
        
        auto sinks = create_track_sinks();
        for (auto& s : sinks) {
            if (s->get_short_track_type() == dst_type) {
                s->save_track(src_tracks.front(), dst_spec.path);
                return 0;
            }
        }
        
        throw runtime_error(string("no writer for track type ") + dst_type);
    } catch (runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
    
    return 0;
}

void convert_addcmd(cmdparser& cparser)
{
    cmd c;
    c.func = convert_main;
    c.short_usage = "converts tracks from one format to another";
    
    cparser.add("convert", c);
}
