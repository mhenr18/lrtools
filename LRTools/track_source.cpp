#include "track_source.h"
#include "trk_source.h"
#include "sol_source.hpp"
#include "json_source.hpp"
using namespace std;

vector<unique_ptr<track_source>> create_track_sources()
{
    vector<unique_ptr<track_source>> sources;
    sources.emplace_back(new trk_source);
    sources.emplace_back(new sol_source);
    sources.emplace_back(new json_source);
    
    return sources;
}
