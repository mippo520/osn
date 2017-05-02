//
//  osn_spin_lock.hpp
//  osn
//
//  Created by zenghui on 17/4/20.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_spin_lock_hpp
#define osn_spin_lock_hpp
#include "osn.h"

class OsnSpinLock {
public:
    OsnSpinLock();
    ~OsnSpinLock();
    
    void lock();
    oBOOL trylock();
    void unlock();
private:
    oINT32 m_nLock;
};

#endif /* osn_spin_lock_hpp */
