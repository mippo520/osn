//
//  main.cpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include <iostream>
#include "osn_start.h"
#include <unistd.h>
#include <queue>
#include "osn_coroutine_manager.h"
#include <stdio.h>


int main(int argc, const char * argv[]) {

    g_OsnStart.init();
    g_OsnStart.start();
    g_OsnStart.exit();
    
    return 0;
}
