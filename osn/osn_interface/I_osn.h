//
//  I_osn.h
//  osn
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef I_osn_h
#define I_osn_h
#include "osn_type.h"
#include "osn_prepared_statement.h"

class IOsn
{
public:
    template <typename ... T>
    ID_SERVICE startService(const std::string &strServiceName, const T& ... args) const
    {
        OsnPreparedStatement stmt;
        DummyWrapper(stmt.pushBack(args) ... );
        return startService(strServiceName, stmt);
    }
    
    template <typename ... T>
    ID_SESSION send(ID_SERVICE addr, oINT32 type, const T& ... args) const
    {
        OsnPreparedStatement stmt;
        DummyWrapper(stmt.pushBack(args) ... );
        return send(addr, type, stmt);
    }
    
    template <typename ... T>
    SHARED_PTR_STMT call(ID_SERVICE addr, oINT32 type, const T& ... args) const
    {
        OsnPreparedStatement stmt;
        DummyWrapper(stmt.pushBack(args) ... );
        return call(addr, type, stmt);
    }

    template <typename ... T>
    void redirect(ID_SERVICE addr, ID_SERVICE source, oINT32 type, ID_SESSION session, const T& ... args) const
    {
        OsnPreparedStatement stmt;
        DummyWrapper(stmt.pushBack(args) ... );
        redirect(addr, source, type, session, stmt);
    }
    
    template <typename ... T>
    void ret(const T& ... args) const
    {
        OsnPreparedStatement stmt;
        DummyWrapper(stmt.pushBack(args) ... );
        ret(stmt);
    }
    
    template <typename ... T>
    ID_COROUTINE fork(VOID_STMT_FUNC func, const T& ... args) const
    {
        OsnPreparedStatement stmt;
        DummyWrapper(stmt.pushBack(args) ... );
        return fork(func, stmt);
    }
    
    virtual ID_SERVICE startService(const std::string &strServiceName, const OsnPreparedStatement &stmt) const = 0;
    virtual ID_SESSION send(ID_SERVICE addr, oINT32 type, const OsnPreparedStatement &msg) const = 0;
    virtual SHARED_PTR_STMT call(ID_SERVICE addr, oINT32 type, const OsnPreparedStatement &msg) const = 0;
    virtual void redirect(ID_SERVICE addr, ID_SERVICE source, oINT32 type, ID_SESSION session, const OsnPreparedStatement &msg) const = 0;
    virtual void ret(const OsnPreparedStatement &msg) const = 0;
    virtual void exit() const = 0;
    virtual void wait(ID_SERVICE unId = 0) const = 0;
    virtual oBOOL wakeup(ID_SERVICE unId) const = 0;
    virtual void registDispatchFunc(oINT32 nPType, DISPATCH_FUNC funcPtr) const = 0;
    virtual void unregistDispatchFunc(oINT32 nPType) const = 0;
    virtual ID_SERVICE self() const = 0;
    virtual ID_COROUTINE fork(VOID_STMT_FUNC func, const OsnPreparedStatement &stmt) const = 0;
private:

    template <typename ... T>
    void DummyWrapper(T ... t) const {}
};

extern const IOsn *g_Osn;

#endif /* I_osn_h */
