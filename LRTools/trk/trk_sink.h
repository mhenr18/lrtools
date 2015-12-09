#ifndef trk_sink_h
#define trk_sink_h

#include "track_sink.h"

class trk_sink : public track_sink {
public:
    virtual bool available_for_tracks(const std::vector<engine::track>& tracks) override;
    virtual const std::string& get_track_type() const override;
    virtual const std::string& get_short_track_type() const override;
    virtual const std::string& get_track_type_plural() const override;
    virtual void save_track(const engine::track& track, const std::string& path) override;
    virtual std::vector<std::string> save_tracks(const std::vector<engine::track>& tracks,
                                                 const std::string& folder) override;
};

#endif
