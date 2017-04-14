//
//  osn_thread.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_thread_worker_hpp
#define osn_thread_worker_hpp
#include <vector>
#include <mutex>
#include <queue>
#include "osn.h"
#include "osn_thread.h"
#include "osn_coroutine_head.h"

class OsnWorkerThread : public OsnThread {
public:
    OsnWorkerThread();
    ~OsnWorkerThread();
private:
    virtual void work();
};

#endif /* osn_thread_worker_hpp */
