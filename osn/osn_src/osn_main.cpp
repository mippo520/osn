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
#include <stdio.h>

int main(int argc, const char * argv[]) {

    const oINT8 * config_file = NULL ;
    if (argc > 1) {
        config_file = argv[1];
        printf("config_file = %s\n", config_file);
    } else {
        printf("Need a config file. \n");
        return 1;
    }
        
    g_OsnStart.init();
    g_OsnStart.start();
    g_OsnStart.exit();

    return 0;
}
