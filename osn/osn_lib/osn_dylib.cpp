//
//  osn_dylib.c
//  TestService
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn_dylib.h"
#include <unistd.h>
#include <stdio.h>
#include "I_osn_service.h"
#include "I_osn_socket.h"
#include "I_osn_coroutine.h"
#include "osn_service_factory.h"

const IOsn *g_Osn = NULL;
const IOsnService *g_Service = NULL;
const IOsnCoroutine *g_Coroutine = NULL;
const IOsnSocket *g_Socket = NULL;

extern "C"
{
    oBOOL init(const IOsn *pOsn
               , const IOsnService *pService
               , const IOsnCoroutine *pCoroutine
               , const IOsnSocket *pSocket)
    {
        oBOOL bRet = false;
        printf("init!\n");
        if (NULL != pOsn && NULL != pService && NULL != pCoroutine && NULL != pSocket)
        {
            g_Osn = pOsn;
            g_Service = pService;
            g_Coroutine = pCoroutine;
            g_Socket = pSocket;
            bRet = true;
        }
        else
        {
            printf("dylib init error! Global value is NULL!\n");
        }
        return bRet;
    }
    
    IServiceFactory* getFactory()
    {
        printf("getfactory!\n");
        return new OsnServiceFactory();
    }

}

