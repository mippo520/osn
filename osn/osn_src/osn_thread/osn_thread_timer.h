//
//  osn_thread.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_thread_timer_hpp
#define osn_thread_timer_hpp
#include "osn.h"
#include "osn_thread.h"

class OsnTimerThread : public OsnThread {
public:
    OsnTimerThread();
    ~OsnTimerThread();
private:
    virtual void onWork();
private:

};

#endif /* osn_thread_timer_hpp */
