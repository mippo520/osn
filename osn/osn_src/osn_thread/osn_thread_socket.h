//
//  osn_thread_socket.hpp
//  osn
//
//  Created by zenghui on 17/4/25.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_thread_socket_hpp
#define osn_thread_socket_hpp
#include "osn.h"
#include "osn_thread.h"

class OsnSocketThread : public OsnThread {
public:
    OsnSocketThread();
    ~OsnSocketThread();
private:
    virtual void onWork();
};

#endif /* osn_thread_socket_hpp */
