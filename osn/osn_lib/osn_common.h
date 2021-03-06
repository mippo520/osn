//
//  osn_common.h
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_common_h
#define osn_common_h

#include <stdlib.h>
#include <assert.h>
#include "osn_type.h"
#include "osn_macro.h"
#include "I_osn.h"

typedef OsnPreparedStatement OSN_CO_ARG;
typedef std::function<SHARED_PTR_STMT(ID_COROUTINE, SHARED_PTR_STMT)> OSN_COROUTINE_FUNC;

#endif /* osn_common_h */
