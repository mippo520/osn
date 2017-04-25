//
//  osn_spin_lock.cpp
//  osn
//
//  Created by zenghui on 17/4/20.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_spin_lock.h"

OsnSpinLock::OsnSpinLock()
    : m_nLock(0)
{
    
}

OsnSpinLock::~OsnSpinLock()
{
    
}

void OsnSpinLock::lock()
{
    while (__sync_lock_test_and_set(&m_nLock,1)) {}
}

oBOOL OsnSpinLock::trylock()
{
    return __sync_lock_test_and_set(&m_nLock,1) == 0;
}

void OsnSpinLock::unlock()
{
    __sync_lock_release(&m_nLock);
}

