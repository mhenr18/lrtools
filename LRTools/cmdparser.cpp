#include <algorithm>
#include <iostream>
#include <vector>
#include "cmdparser.hpp"
#include "util.h"
using namespace std;

void cmdparser::add(std::string name, cmd c)
{
    cmds.insert({move(name), c});
}

int cmdparser::run(int argc, const char **argv)
{
    if (argc == 1) {
        usage();
        return 1;
    }
    
    if (cmds.count(argv[1])) {
        return cmds[argv[1]].func(argc - 1, argv + 1);
    } else {
        cout << "unknown command `" << argv[1] << "'" << endl;
        return 1;
    }
}

void cmdparser::usage()
{
    vector<string> names;
    int max_len = 0;
    
    for (auto& p : cmds) {
        if (p.first.length() > max_len) {
            max_len = (int)p.first.length();
        }
        
        names.push_back(p.first);
    }
    
    sort(names.begin(), names.end());
    
    cout << "usage:" << endl;
    for (auto& n : names) {
        cout << "   " << pre_pad(n, max_len) << "    " << cmds[n].short_usage << endl;
    }
}
