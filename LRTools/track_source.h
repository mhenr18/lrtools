#ifndef track_source_h
#define track_source_h

#include <memory>
#include <string>
#include <vector>
#include "track.h"

class track_source {
public:
    virtual ~track_source() {};
    
    // should be human readable, i.e "Beta 2 Track" or "LineOnline Track"
    virtual const std::string& get_track_type() const = 0;
    virtual const std::string& get_short_track_type() const = 0;
    
    // can be an empty vector if there were no tracks that this source could read
    // from the given path (note that this should be nothrow - just return an empty
    // vector on error)
    virtual std::vector<engine::track> load_tracks(const std::string& path) = 0;
};

// we return a new instance of each different track source type
std::vector<std::unique_ptr<track_source>> create_track_sources();

#endif
