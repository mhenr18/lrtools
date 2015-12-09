#ifndef json_source_hpp
#define json_source_hpp

#include "track_source.h"

class json_source : public track_source {
public:
    virtual const std::string& get_track_type() const override;
    virtual const std::string& get_short_track_type() const override;
    
    virtual std::vector<engine::track> load_tracks(const std::string& path) override;
};

#endif
