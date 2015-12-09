#ifndef engine_line_h
#define engine_line_h

#include "vec2.h"

namespace engine {
    
    typedef int32_t line_id;
    
    enum line_type {
        LT_NORMAL,
        LT_ACCELERATION,
        LT_SCENERY
    };
    
    struct line {
        vec2d p0, p1;
        line_type type;
        bool inverse;
        line_id name; // I'd love to call this 'id' but we need to work with Objective-C++
        
        uint8_t snap_mode;
        line_id p0_snap, p1_snap;
        
        bool marked;
    };
    
}

#endif
