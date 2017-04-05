//
//  osn_thread.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_thread_worker_hpp
#define osn_thread_worker_hpp
#include "osn.h"
#include "osn_thread.h"

class OsnWorkerThread : public OsnThread {
public:
    OsnWorkerThread();
    ~OsnWorkerThread();
private:
    virtual void work();
private:

};

#endif /* osn_thread_worker_hpp */
