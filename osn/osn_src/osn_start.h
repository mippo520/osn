//
//  osn_start.hpp
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_start_hpp
#define osn_start_hpp
#include "osn_singleton.h"
#include "osn.h"
#include <vector>

class OsnThread;

class OsnStart {
    const static oINT32 s_nWeightCount = 32;
    static int s_WeightArr[s_nWeightCount];
    static oINT32 s_nThreadIdIdx;
    
    OsnStart();
public:
    const static oINT32 s_nWorkerCount = 8;
public:
    friend class Singleton<OsnStart>;
    ~OsnStart();
public:
    void init();
    void start();
    void exit();
    
    oBOOL checkAbort();
    void wakeup(oINT32 nSleepCount);
private:
    void clearThread();
    
    template<class T>
    void createThread(oINT32 nWeight = 0)
    {
        T *pThread = new T();
        pThread->setId(++s_nThreadIdIdx);
        pThread->setWeight(nWeight);
        m_vecThread.push_back(pThread);
    }
private:
    std::vector<OsnThread*> m_vecThread;
};

#define g_OsnStart Singleton<OsnStart>::instance()

#endif /* osn_start_hpp */
