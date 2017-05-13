//
//  osn.cpp
//  osn
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#include "osn.h"
#include <dlfcn.h>
#include "osn_service_manager.h"
#include "osn_service.h"
#include "osn_coroutine_manager.h"
#include "I_osn_socket.h"


const IOsn *g_Osn = &OsnSingleton<Osn>::instance();

Osn::Osn()
{
    
}

Osn::~Osn()
{
    
}

oBOOL Osn::loadService(const std::string &strServiceName) const
{
    oBOOL bRet = false;
    
    do {
        typedef oBOOL (*DYLIB_INIT)(const IOsn *pOsn, const IOsnService *pService, const IOsnCoroutine *pCoroutine, const IOsnSocket *pSocket);
        typedef IServiceFactory* (*DYLIB_GET_FACTORY)();
        
        std::string strFullPath = "./lib";
        strFullPath += strServiceName;
#ifdef __APPLE__
        strFullPath += ".dylib";
#elif defined __linux__
        strFullPath += ".so";
#endif
        
        void *handle = dlopen(strFullPath.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (NULL == handle)
        {
            printf("Osn::loadService Error! open %s error! %s\n", strFullPath.c_str(), dlerror());
            break;
        }
        
        DYLIB_INIT init = (DYLIB_INIT)dlsym(handle, "init");
        if (NULL == init)
        {
            printf("Osn::loadService Error! loal function init error! %s\n", dlerror());
            break;
        }
        (*init)(this, g_Service, g_Coroutine, g_Socket);
        
        DYLIB_GET_FACTORY getFactory = (DYLIB_GET_FACTORY)dlsym(handle, "getFactory");
        if (NULL == getFactory)
        {
            printf("Osn::loadService Error! loal function getFactory error! %s\n", dlerror());
            break;
        }
        IServiceFactory *pFactory = (*getFactory)();
        g_ServiceManager.addServiceFactory(strServiceName, pFactory);
        g_ServiceManager.pushDylibHandle(handle);
        bRet = true;
    } while (false);
    
    return bRet;
}

oUINT32 Osn::startService(const std::string &strServiceName) const
{
    oUINT32 unId = g_ServiceManager.startService(strServiceName);
    do {
        if (unId > 0)
        {
            break;
        }
        if (!loadService(strServiceName))
        {
            break;
        }
        unId = g_ServiceManager.startService(strServiceName);
        if (0 == unId)
        {
            printf("Osn::startService Error! %s can not found!", strServiceName.c_str());
        }
    } while (false);

    return unId;
}

oUINT32 Osn::send(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg) const
{
    return g_ServiceManager.sendMessage(addr, 0, type, 0, &msg);
}

const OsnPreparedStatement& Osn::call(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg) const
{
    oUINT32 unSession = g_ServiceManager.sendMessage(addr, g_ServiceManager.getCurService(), type, 0, &msg);
    OSN_CO_ARG arg;
    arg.setUInt32(0, unSession);
    arg.pushBackInt32(OsnService::eYT_Call);
    return g_CorotineManager.yield(arg);
}

void Osn::ret(const OsnPreparedStatement &msg) const
{
    msg.pushBackInt32(OsnService::eYT_Return);
    g_CorotineManager.yield(msg);
}

void Osn::exit() const
{
    OsnPreparedStatement stmt;
    stmt.pushBackInt32(OsnService::eYT_Quit);
    g_CorotineManager.yield(stmt);
}

void Osn::wait(oUINT32 unId) const
{
    oUINT32 unSession = g_ServiceManager.genId();
    OsnPreparedStatement stmt;
    stmt.pushBackUInt32(unSession);
    stmt.pushBackInt32(OsnService::eYT_Sleep);
    g_CorotineManager.yield(stmt);
    
    stmt.clear();
    stmt.pushBackUInt32(unSession);
    stmt.pushBackInt32(OsnService::eYT_CleanSleep);
    g_CorotineManager.yield(stmt);
}

oBOOL Osn::wakeup(oUINT32 unId) const
{
    OsnPreparedStatement stmt;
    stmt.pushBackUInt32(unId);
    stmt.pushBackInt32(OsnService::eYT_Wakeup);
    const OsnPreparedStatement &ret = g_CorotineManager.yield(stmt);
    return ret.getBool(0);
}

void Osn::registDispatchFunc(oINT32 nPType, VOID_STMT_FUNC funcPtr) const
{
    g_ServiceManager.registDispatchFunc(nPType, funcPtr);
}

void Osn::unregistDispatchFunc(oINT32 nPType) const
{
    g_ServiceManager.unregistDispatchFunc(nPType);
}

