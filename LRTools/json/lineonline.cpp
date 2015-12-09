#include <vector>
#include <stdexcept>
#include <iostream>
#include "json.h"
#include "lineonline.h"
using namespace engine;
using namespace std;

std::string engine::lineonline_save(const track& t)
{
    Json::Value root = Json::objectValue;
    root["label"] = t.get_name();
    root["version"] = "6.2";
    
    if (t.get_start_pos() != vec2d(0, 0)) {
        root["startPosition"] = Json::objectValue;
        root["startPosition"]["x"] = t.get_start_pos().x;
        root["startPosition"]["y"] = t.get_start_pos().y;
    }
    
    root["lines"] = Json::arrayValue;
    
    for (auto& l : t.get_lines()) {
        Json::Value line = Json::objectValue;
        line["id"] = l.name;
        line["type"] = (int)l.type;
        line["x1"] = l.p0.x;
        line["y1"] = l.p0.y;
        line["x2"] = l.p1.x;
        line["y2"] = l.p1.y;
        line["extended"] = (int)l.snap_mode;
        line["flipped"] = (l.inverse ? 1 : 0);
        
        if (l.p0_snap != -1) {
            line["leftLine"] = l.p0_snap;
        }
        
        if (l.p1_snap != -1) {
            line["rightLine"] = l.p1_snap;
        }
        
        root["lines"].append(line);
    }
    
    Json::StyledWriter writer;
    return writer.write(root);
}

track engine::lineonline_load(const uint8_t *buf, size_t bufsize)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse((const char *)buf, (const char *)(buf + bufsize), root);
    
    if (root.get("version", "X").asString() != "6.2") {
        throw runtime_error("invalid version");
    }
    
    if (root["label"].isNull()) {
        throw runtime_error("no label");
    }
    
    vec2d start_pos = {0, 0};
    if (!root["startPosition"].isNull()) {
        start_pos = {root["startPosition"]["x"].asDouble(),
            root["startPosition"]["y"].asDouble()};
    }
    
    string label = root["label"].asString();
    
    vector<line> lines;
    for (auto& l : root["lines"]) {
        line line;
        line.name = l["id"].asInt();
        line.inverse = l["flipped"].asInt() != 0;
        line.marked = false;
        line.type = (line_type)l["type"].asInt();
        line.p0 = {l["x1"].asDouble(), l["y1"].asDouble()};
        line.p1 = {l["x2"].asDouble(), l["y2"].asDouble()};
        line.snap_mode = (uint8_t)l["extended"].asInt();
        line.p0_snap = l.get("leftLine", -1).asInt();
        line.p1_snap = l.get("rightLine", -1).asInt();
        
        lines.push_back(line);
    }
    
    return track(std::move(lines), start_pos, label);
}