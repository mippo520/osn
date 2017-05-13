//
//  osn_dylib.h
//  TestService
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_dylib_h
#define osn_dylib_h
#include <string>
#include "osn_common.h"

class IOsn;
class IOsnService;
class IOsnCoroutine;
class IOsnSocket;
class IServiceFactory;
class OsnServiceFactory;

extern "C"
{
    oBOOL init(const IOsn *pOsn, const IOsnService *pService, const IOsnCoroutine *pCoroutine, const IOsnSocket *pSocket);
    IServiceFactory* getFactory();
}

#endif /* osn_dylib_h */
