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
    virtual oBOOL loadService(const std::string &strServiceName) const = 0;
    virtual oUINT32 startService(const std::string &strServiceName) const = 0;
    virtual oUINT32 send(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg = OsnPreparedStatement()) const = 0;
    virtual const OsnPreparedStatement& call(oUINT32 addr, oINT32 type, const OsnPreparedStatement &msg = OsnPreparedStatement()) const = 0;
    virtual void ret(const OsnPreparedStatement &msg) const = 0;
    virtual void exit() const = 0;
    virtual void wait(oUINT32 unId = 0) const = 0;
    virtual oBOOL wakeup(oUINT32 unId) const = 0;
    virtual void registDispatchFunc(oINT32 nPType, VOID_STMT_FUNC funcPtr) const = 0;
    virtual void unregistDispatchFunc(oINT32 nPType) const = 0;
};

extern const IOsn *g_Osn;

#endif /* I_osn_h */
