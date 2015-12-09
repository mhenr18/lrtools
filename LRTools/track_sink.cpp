#include "track_sink.h"
#include "trk_sink.h"
#include "sol_sink.hpp"
#include "json_sink.h"
using namespace std;

vector<unique_ptr<track_sink>> create_track_sinks()
{
    vector<unique_ptr<track_sink>> sinks;
    sinks.emplace_back(new trk_sink);
    sinks.emplace_back(new sol_sink);
    sinks.emplace_back(new json_sink);
    
    return sinks;
}

