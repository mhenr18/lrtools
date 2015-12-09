#include <iostream>
#include "sols.hpp"
#include "platform.hpp"
#include "util.h"
using namespace std;

static int sols_main(int argc, const char **argv)
{
#ifdef __APPLE__
    string shared_objs_path = expand_path("~/Library/Preferences/Macromedia/Flash\\ Player/#SharedObjects/*");
    
    cout << shared_objs_path << endl;
#endif
    
    return 0;
}

void sols_addcmd(cmdparser& cparser)
{
    cmd c;
    c.func = sols_main;
    c.short_usage = "prints the path to the sols folder";
    
    cparser.add("sols", c);
}
