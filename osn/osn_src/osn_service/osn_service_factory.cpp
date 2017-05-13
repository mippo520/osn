//
//  osn_service_factory.cpp
//  TestService
//
//  Created by zenghui on 17/5/13.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_service_factory.h"
#include "service.h"

OsnService* OsnServiceFactory::create()
{
    return new Service();
}

