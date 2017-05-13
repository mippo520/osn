//
//  osn_service_factory.hpp
//  TestService
//
//  Created by zenghui on 17/5/13.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_service_factory_hpp
#define osn_service_factory_hpp

#include "I_osn_service_factory.h"

class OsnServiceFactory : public IServiceFactory
{
public:
    virtual OsnService* create();
};

#endif /* osn_service_factory_hpp */
