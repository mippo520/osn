//
//  main.cpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include <iostream>
#include "osn_start.h"
#include <unistd.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    g_OsnStart.init();
    g_OsnStart.start();
    g_OsnStart.exit();

    return 0;
}
