//
//  main.cpp
//  LRTools
//
//  Created by Matthew Henry on 8/12/2015.
//  Copyright © 2015 Matthew Henry. All rights reserved.
//

#include <iostream>
#include "cmdparser.hpp"
#include "ls.hpp"
#include "sols.hpp"
#include "convert.hpp"
#include "dump.hpp"

int main(int argc, const char * argv[])
{
    cmdparser cparser;
    ls_addcmd(cparser);
    sols_addcmd(cparser);
    convert_addcmd(cparser);
    dump_addcmd(cparser);
    
    return cparser.run(argc, argv);
}
