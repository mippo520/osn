//
//  TestService3.hpp
//  osn
//
//  Created by zenghui on 17/5/3.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef TestService3_hpp
#define TestService3_hpp

#include "osn_service.h"

class TestService3 : public OsnService
{
public:
    TestService3();
    ~TestService3();
public:
    virtual void start(const OsnPreparedStatement &stmt) override;
    virtual void exit() override;
    void dispatchLua(const OsnPreparedStatement &stmt);
};

#endif /* TestService3_hpp */
