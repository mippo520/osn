//
//  gate.cpp
//  gate
//
//  Created by zenghui on 17/5/19.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "gate.h"

AddService_Instance(Gate)

Gate::Gate()
{
    
}

Gate::~Gate()
{
    
}

void Gate::start(const OsnPreparedStatement &stmt)
{
    printf("Gate::start ====>!\n");
}

