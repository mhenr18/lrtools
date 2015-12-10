#include <iostream>
#include <iomanip>
#include "dump.hpp"
#include "platform.hpp"
#include "util.h"
using namespace std;

static string type_to_str(engine::line_type type)
{
    switch (type) {
        case engine::LT_NORMAL:
            return "blue";
        case engine::LT_ACCELERATION:
            return "accel";
        case engine::LT_SCENERY:
            return "scenery";
        default:
            return "???";
    }
}

static int dump_main(int argc, const char **argv)
{
    if (argc < 2) {
        cerr << "usage: lrtools dump <input_track>" << endl;
        return 1;
    }
    
    auto src_spec = parse_specifier(argv[1]);
    
    try {
        auto src_tracks = load_from_specifier(src_spec);
        if (src_tracks.size() > 1) {
            throw runtime_error("unable to dump multiple tracks at once");
        }
        
        auto& t = src_tracks.front();
        
        cout << t.get_name() << endl;
        
        cout << "    id    type snap p0_snap p1_snap inverse           x1           y1           x2           y2" << endl;
        
        for (auto& l : t.get_lines()) {
            cout << pre_pad(strfmt() << l.name, 6)
                 << " " << pre_pad(type_to_str(l.type), 7)
                 << "    " << (unsigned)l.snap_mode
                 << " " << pre_pad(strfmt() << l.p0_snap, 7)
                 << " " << pre_pad(strfmt() << l.p1_snap, 7)
                 << " " << pre_pad(l.inverse ? "true" : "false", 7)
                 << " " << pre_pad(strfmt() << fixed << setprecision(3) << l.p0.x, 12)
                 << " " << pre_pad(strfmt() << fixed << setprecision(3) << l.p0.y, 12)
                 << " " << pre_pad(strfmt() << fixed << setprecision(3) << l.p1.x, 12)
                 << " " << pre_pad(strfmt() << fixed << setprecision(3) << l.p1.y, 12)
                 << endl;
        }
        
    } catch (runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
    
    return 0;
}

void dump_addcmd(cmdparser& cparser)
{
    cmd c;
    c.func = dump_main;
    c.short_usage = "dumps a track's line information";
    
    cparser.add("dump", c);
}
