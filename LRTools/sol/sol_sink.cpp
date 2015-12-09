#include "amf0.h"
#include "sol_sink.hpp"
#include "util.h"
using namespace engine;
using namespace std;

static string track_type = "Beta 2 Track";
static string short_track_type = "Beta2";
static string plural_track_type = "Beta 2 Tracks";

// TODO: this is some insanely leak-filled code. needs a complete rewrite....

char * copy_str(string str)
{
    char *buf = (char *)malloc(str.length() + 1);
    strcpy(buf, str.c_str());
    return buf;
}

amf0_obj * make_amf0_obj(string name, string value)
{
    amf0_obj* obj = new amf0_obj;
    obj->name = copy_str(name);
    obj->type = AMF0_STRING;
    obj->data.string = copy_str(value);
    obj->next = NULL;
    
    return obj;
}

amf0_obj * make_amf0_obj(string name)
{
    amf0_obj* obj = new amf0_obj;
    obj->name = copy_str(name);
    obj->type = AMF0_OBJECT;
    obj->data.properties = NULL;
    obj->next = NULL;
    
    return obj;
}

amf0_obj * make_amf0_obj(int name)
{
    amf0_obj* obj = new amf0_obj;
    obj->name = (char *)malloc(16);
    sprintf(obj->name, "%d", name);
    obj->type = AMF0_OBJECT;
    obj->data.properties = NULL;
    obj->next = NULL;
    
    return obj;
}

amf0_obj * make_amf0_obj(string name, double value)
{
    amf0_obj* obj = new amf0_obj;
    obj->name = copy_str(name);
    obj->type = AMF0_NUMBER;
    obj->data.number = value;
    obj->next = NULL;
    
    return obj;
}

amf0_obj * make_amf0_obj(int name, double value)
{
    amf0_obj* obj = new amf0_obj;
    obj->name = (char *)malloc(16);
    sprintf(obj->name, "%d", name);
    obj->type = AMF0_NUMBER;
    obj->data.number = value;
    obj->next = NULL;
    
    return obj;
}

bool sol_sink::available_for_tracks(const std::vector<engine::track>& tracks)
{
    // sol is always available (at the moment)
    return true;
}

const std::string& sol_sink::get_track_type() const
{
    return track_type;
}

const std::string& sol_sink::get_short_track_type() const
{
    return short_track_type;
}

const std::string& sol_sink::get_track_type_plural() const
{
    return plural_track_type;
}

static void save_tracks(const std::vector<engine::track>& tracks,
                        const std::string& path)
{
    amf0_obj trackList;
    trackList.name = "trackList";
    trackList.next = NULL;
    trackList.type = AMF0_ECMA_ARRAY;
    trackList.length = (uint32_t)tracks.size();
    trackList.data.properties = NULL;
    amf0_obj *curr = NULL;
    
    for (int i = 0; i < tracks.size(); ++i) {
        auto *t = tracks.data() + i;
        line_id max_id = 1;
        if (t->line_count()) {
            max_id = t->get_lines()[t->line_count() - 1].name + 1;
        }
        
        auto *level = make_amf0_obj("level", (double)max_id);
        auto *version = make_amf0_obj("version", "6.2");
        auto *startLine = make_amf0_obj("startLine");
        auto *sx = make_amf0_obj(0, t->get_start_pos().x);
        auto *sy = make_amf0_obj(1, t->get_start_pos().y);
        startLine->data.properties = sx;
        sx->next = sy;
        
        amf0_obj* track_obj = new amf0_obj;
        char *namebuf = (char *)malloc(16);
        sprintf(namebuf, "%d", i);
        track_obj->name = namebuf;
        track_obj->type = AMF0_OBJECT;
        track_obj->data.properties = version;
        version->next = startLine;
        startLine->next = level;
        
        auto *data = make_amf0_obj("data");
        data->type = AMF0_ECMA_ARRAY;
        level->next = data;
        data->length = (uint32_t)t->get_lines().size();
        data->data.properties = NULL;
        amf0_obj *curr_line = NULL;
        
        for (int j = t->get_lines().size() - 1; j >= 0; --j) {
            const auto *l = t->get_lines().data() + j;
            auto *line_obj = make_amf0_obj(j);
            auto *x1 = make_amf0_obj(0, l->p0.x);
            auto *y1 = make_amf0_obj(1, l->p0.y);
            auto *x2 = make_amf0_obj(2, l->p1.x);
            auto *y2 = make_amf0_obj(3, l->p1.y);
            auto *snap_mode = make_amf0_obj(4, l->snap_mode);
            auto *inverse = make_amf0_obj(5, l->inverse ? 1 : 0);
            auto *p0_snap = make_amf0_obj(6, l->p0_snap != -1 ? l->p0_snap : NAN);
            auto *p1_snap = make_amf0_obj(7, l->p1_snap != -1 ? l->p1_snap : NAN);
            auto *name = make_amf0_obj(8, l->name);
            auto *type = make_amf0_obj(9, (double)l->type);
            
            x1->next = y1;
            y1->next = x2;
            x2->next = y2;
            y2->next = snap_mode;
            snap_mode->next = inverse;
            inverse->next = p0_snap;
            p0_snap->next = p1_snap;
            p1_snap->next = name;
            name->next = type;
            
            line_obj->length = 10;
            line_obj->type = AMF0_ECMA_ARRAY;
            line_obj->data.properties = x1;
            
            if (curr_line) {
                curr_line->next = line_obj;
            } else {
                data->data.properties = line_obj;
            }
            
            curr_line = line_obj;
        }
        
        auto *label = make_amf0_obj("label", t->get_name());
        data->next = label;
        
        if (curr) {
            curr->next = track_obj;
        } else {
            trackList.data.properties = track_obj;
        }
        
        curr = track_obj;
    }
    
    amf0_obj root;
    root.name = "savedLines";
    root.next = NULL;
    root.data.properties = &trackList;
    
    write_sol(&root, path.c_str());
}

void sol_sink::save_track(const engine::track& track, const std::string& path)
{
    ::save_tracks(vector<engine::track> { track }, path);
}

std::vector<std::string> sol_sink::save_tracks(const std::vector<engine::track>& tracks,
                                               const std::string& folder)
{
    throw runtime_error("not implemented");
}
