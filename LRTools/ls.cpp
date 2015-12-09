#include <iostream>
#include "ls.hpp"
#include "OptionParser.hpp"
#include "platform.hpp"
#include "track_source.h"
#include "util.h"
using namespace std;
using namespace optparse;

// if there's more than one track in a file, you need to use an extended
// path to refer to the track (i.e savedLines.sol:trackname), which this
// function can generate. the extended path is the minimum path required
// to uniquely prefix the track's name.
static string get_extpath(const vector<engine::track>& tracks, engine::track& t)
{
    if (tracks.size() == 1) {
        return "";
    }
    
    for (size_t prefix_len = 1; prefix_len < t.get_name().length(); ++prefix_len) {
        string prefix = t.get_name().substr(0, prefix_len);
        bool ok = true;
        
        for (auto& track : tracks) {
            if (track.get_name() == t.get_name()) {
                continue;
            }
            
            if (track.get_name().find(prefix) == 0) {
                ok = false;
            }
        }
        
        if (ok) {
            return string(":") + prefix;
        }
    }
    
    return string(":") + t.get_name();
}

static size_t ls_file(const string& path)
{
    auto sources = create_track_sources();
    
    for (auto& src : sources) {
        auto tracks = src->load_tracks(path);
        
        if (tracks.size() > 0) {
            for (auto& t : tracks) {
                if (true) {
                    cout << post_pad(limit_length(t.get_name(), 20), 20)
                         << " " << pre_pad(strfmt() << t.line_count(), 6)
                         << " " << pre_pad(src->get_short_track_type(), 5)
                         << " " << split_path(path).back() + get_extpath(tracks, t)
                         << endl;
                } else {
                    cout << "" << t.get_name() << endl;
                }
            }
        
            return tracks.size();
        }
    }
    
    return 0;
}

static void ls_directory(const string& path)
{
    bool have_tracks = false;
    
    for (auto& p : list_directory(path)) {
        if (is_file(p)) {
            auto ntracks = ls_file(p);
            if (ntracks) {
                have_tracks = true;
            }
        }
    }
    
    if (!have_tracks) {
        //cerr << "no tracks in `" << path << "'" << endl;
    }
}

static bool ls_path(const string& path)
{
    if (is_file(path)) {
        ls_file(path);
    } else if (is_directory(path)) {
        ls_directory(path);
    } else {
        return false;
    }
    
    return true;
}

static int ls_main(int argc, const char **argv)
{
    if (argc == 1) {
        ls_directory(expand_path("."));
        return 1;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (argc > 2) {
            cout << argv[i] << ":" << endl;
        }
        
        if (!ls_path(argv[i])) {
            cerr << "unknown path `" << argv[i] << "'" << endl;
            return 1;
        } else {
            cout << endl;
        }
    }
    
    return 0;
}

void ls_addcmd(cmdparser& cparser)
{
    cmd ls_cmd;
    ls_cmd.func = ls_main;
    ls_cmd.short_usage = "lists all tracks in the given files/directories";
    
    cparser.add("ls", ls_cmd);
}
