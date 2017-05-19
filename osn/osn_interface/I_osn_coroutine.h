//
//  I_osn_coroutine.h
//  osn
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef I_osn_coroutine_h
#define I_osn_coroutine_h
#include "osn_type.h"

class IOsnCoroutine
{
public:
    virtual ID_COROUTINE create(const OSN_COROUTINE_FUNC &func) const = 0;
    virtual oBOOL destroy(ID_COROUTINE co) const = 0;
    virtual const OSN_CO_ARG& yield(const OSN_CO_ARG &arg = OSN_CO_ARG()) const = 0;
    virtual const OSN_CO_ARG& resume(ID_COROUTINE unId, const OSN_CO_ARG &arg = OSN_CO_ARG()) const = 0;
    virtual ID_COROUTINE running() const = 0;
};

extern const IOsnCoroutine *g_Coroutine;

#endif /* I_osn_coroutine_h */
