//
//  osn_thread.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_thread_hpp
#define osn_thread_hpp
#include "osn.h"
#include <thread>

class OsnThread {
protected:
    static oBOOL s_isQuit;
public:
    OsnThread();
    ~OsnThread();
public:
    void init(oBOOL isMainWait = true);
    void setId(oINT32 id);
    oINT32 getId();
private:
    virtual void work();
private:
    std::thread m_Thread;
    oINT32 m_nId;
};

#endif /* osn_thread_hpp */
