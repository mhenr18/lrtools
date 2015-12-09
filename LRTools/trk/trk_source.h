#ifndef trk_source_h
#define trk_source_h

#include "track_source.h"

class trk_source : public track_source {
public:
    virtual const std::string& get_track_type() const override;
    virtual const std::string& get_short_track_type() const override;
    
    virtual std::vector<engine::track> load_tracks(const std::string& path) override;
};

#endif
