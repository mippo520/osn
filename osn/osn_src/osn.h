//
//  osn.hpp
//  osn
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_hpp
#define osn_hpp
#include "I_osn.h"
#include "osn_singleton.h"
#include "osn_spin_lock.h"

class Osn : public IOsn
{
    friend class OsnSingleton<Osn>;
    Osn();
public:
    ~Osn();
    virtual ID_SERVICE startService(const std::string &strServiceName, const OsnPreparedStatement &stmt = OsnPreparedStatement()) const;
    virtual ID_SESSION send(ID_SERVICE addr, oINT32 type, const OsnPreparedStatement &msg = OsnPreparedStatement()) const;
    virtual const OsnPreparedStatement& call(ID_SERVICE addr, oINT32 type, const OsnPreparedStatement &msg = OsnPreparedStatement()) const;
    virtual void redirect(ID_SERVICE addr, ID_SERVICE source, oINT32 type, ID_SESSION session, const OsnPreparedStatement &msg) const;
    virtual void ret(const OsnPreparedStatement &msg) const;
    virtual void exit() const;
    virtual void wait(ID_SERVICE unId = 0) const;
    virtual oBOOL wakeup(ID_SERVICE unId) const;
    virtual void registDispatchFunc(oINT32 nPType, DISPATCH_FUNC funcPtr) const;
    virtual void unregistDispatchFunc(oINT32 nPType) const;
private:
    virtual oBOOL loadService(const std::string &strServiceName) const;
private:
    mutable OsnSpinLock m_LoadLock;
};

#endif /* osn_hpp */
