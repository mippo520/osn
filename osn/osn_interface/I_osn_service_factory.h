//
//  I_osn_service_factory.h
//  osn
//
//  Created by zenghui on 17/5/13.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef I_osn_service_factory_h
#define I_osn_service_factory_h

class OsnService;

class IServiceFactory
{
public:
    virtual ~IServiceFactory() {}
    virtual OsnService* create() = 0;
};

#endif /* I_osn_service_factory_h */
