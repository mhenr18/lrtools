#ifndef track_sink_h
#define track_sink_h

#include <memory>
#include <string>
#include <string>
#include <vector>
#include "track.h"

class track_sink {
public:
    virtual ~track_sink() {};
    
    // whether this sink can write the given tracks
    // (i.e the Beta 2 multiple sol sink isn't applicable to only 1 track)
    virtual bool available_for_tracks(const std::vector<engine::track>& tracks) = 0;
    
    // should be human readable, i.e "Beta 2 Track" or "LineOnline Track"
    virtual const std::string& get_track_type() const = 0;
    
    virtual const std::string& get_short_track_type() const = 0;
    
    // the plural version of get_track_type()
    virtual const std::string& get_track_type_plural() const = 0;
    
    virtual void save_track(const engine::track& track, const std::string& path) = 0;
    
    // should write the given tracks into the given folder. some sinks might
    // need to use multiple files, some might be able to write them into a
    // single .sol. if there's an error in writing, just return an empty vector.
    virtual std::vector<std::string> save_tracks(const std::vector<engine::track>& tracks,
                                                 const std::string& folder) = 0;
};

// we return a new instance of each different track sink type
std::vector<std::unique_ptr<track_sink>> create_track_sinks();

#endif
