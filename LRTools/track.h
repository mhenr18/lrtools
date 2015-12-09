#ifndef engine_track_h
#define engine_track_h

#include <array>
#include <string>
#include <unordered_map>
#include <vector>
#include "grid.h"
#include "line.h"

// a beta 2 compliant implementation of a Line Rider track.
//
// rather than allocate each line individually and access by reference, we store
// them directly in a vector. we then refer to lines by binary searching based
// on their ids.
//
// this decision makes individual line access slightly slower, but makes iteration
// over all of the lines much faster because we don't cache miss all of the time.

namespace engine {
    
    class track {
    public:
        track();
        track(std::vector<line> lines, vec2d start_pos, std::string name);
        
        // only valid as long as you don't alter the track
        const line& get_line(line_id name) const;
        const std::vector<line>& get_lines() const;
        const std::string& get_name() const;
        
        // allocates a new id for the line (ignoring any existing one) and inserts it
        line_id add_new_line(line l);
        
        // attempts to reuse the line's id. this is so that undo/redo can work correctly,
        // without breaking the physics (if we just redo a line deletion by adding it as
        // new, that ends up with a different collision evaluation order and breaks tracks)
        line_id insert_line(line l);
        
        // rather than being able to directly delete lines, we use a mark-sweep approach.
        // this allows for efficient deletion of multiple lines. without this, deleting L
        // lines from a track of N lines would be O(L * N), whereas this approach allows
        // us to do it in much closer to O(N) regardless of the number of lines.
        void mark(line_id lid);
        void sweep();
        
        int line_count() const;
        int line_count(line_type type) const;
        
        void set_start_pos(vec2d start_pos);
        vec2d get_start_pos() const;
        
        void set_name(std::string new_name);
    private:
        line* get_line_ptr(line_id name);
        const line* get_line_ptr(line_id name) const;
        
        void register_line(const line& line);
        void deregister_line(const line& line);
        
        std::string name;
        std::vector<line> lines;
        grid colliding_grid, scenery_grid;
        std::array<int, 3> line_counts;
        vec2d start_pos;
    };
    
}

#endif
