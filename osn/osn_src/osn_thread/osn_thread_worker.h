//
//  osn_thread.hpp
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_thread_worker_hpp
#define osn_thread_worker_hpp
#include "osn_common.h"
#include "osn_thread.h"

class OsnWorkerThread : public OsnThread {
public:
    OsnWorkerThread();
    ~OsnWorkerThread();
private:
    virtual void onWork();
};

#endif /* osn_thread_worker_hpp */
