//
//  gate.hpp
//  gate
//
//  Created by zenghui on 17/5/19.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef gate_hpp
#define gate_hpp
#include "osn_service.h"
#include "I_osn_service_factory.h"

class Gate : public OsnService
{
public:
    Gate();
    ~Gate();
    virtual void start(const OsnPreparedStatement &stmt) override;
};

AddService_Declare(Gate)

#endif /* gate_hpp */
