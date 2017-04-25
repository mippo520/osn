//
//  osn_poll.hpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_poll_hpp
#define osn_poll_hpp
#include "osn.h"

class OsnPoll
{
public:
    virtual ~OsnPoll() {}
    
    virtual oINT32 create() = 0;
    virtual oBOOL invalid(oINT32 poll_fd) = 0;
    virtual void release(oINT32 poll_fd) = 0;
    virtual oBOOL add(oINT32 poll_fd, oINT32 sock, void *ud) = 0;
    virtual void del(oINT32 poll_fd, oINT32 sock) = 0;
};

#endif /* osn_poll_hpp */
