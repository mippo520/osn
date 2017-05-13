//
//  osn_service_factory.hpp
//  osn
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_service_factory_hpp
#define osn_service_factory_hpp

class OsnService;

class OsnServiceFactory
{
public:
    virtual ~OsnServiceFactory() {}
    virtual OsnService* create() = 0;
};


#endif /* osn_service_factory_hpp */
