//
//  osn_socket.hpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_socket_hpp
#define osn_socket_hpp
#include "osn.h"

class OsnSocket
{
public:
    OsnSocket();
    ~OsnSocket();
private:
    MEMBER_VALUE(oINT32, Type);
    MEMBER_VALUE(oINT32, Id);
};

#endif /* osn_socket_hpp */
