#ifndef cmdparser_hpp
#define cmdparser_hpp

#include <functional>
#include <string>
#include <unordered_map>

struct cmd {
    std::function<int(int, const char **)> func;
    std::string short_usage;
};

class cmdparser {
public:
    void add(std::string name, cmd c);
    
    int run(int argc, const char **argv);
private:
    void usage();
    
    std::unordered_map<std::string, cmd> cmds;
};

#endif
