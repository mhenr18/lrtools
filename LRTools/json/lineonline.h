#ifndef engine_lineonline_h
#define engine_lineonline_h

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include "track.h"

// lineonline.h
// functions for loading and saving tracks in json files that
// are compatible with lineonline

namespace engine {
    
    std::string lineonline_save(const track& t);
    
    // loading
    track lineonline_load(const uint8_t *buf, size_t bufsize);
}

#endif
