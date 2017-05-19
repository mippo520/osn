//
//  osn_arr_manager.hpp
//  osn
//
//  Created by zenghui on 17/4/10.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_arr_manager_hpp
#define osn_arr_manager_hpp
#include <vector>
#include <queue>
#include <mutex>
#include "osn_common.h"
#include "osn_spin_lock.h"

enum eThreadType
{
    eThread_Normal = 0,
    eThread_Saved,
};

class OsnArrTools
{
    const static oUINT64 s_u64ArrPosMask = 0xffffffff;
public:
    const static oINT32 s_nObjCountBegin = 750;
    static oUINT64 getPos(oUINT64 u64Id)
    {
        return u64Id & s_u64ArrPosMask;
    }
};

template<class T, int ThreadType = eThread_Normal>
class OsnArrManager
{
    const static oUINT64 s_u64RepeatTimeMask = 32;
    static oINT32 s_nObjIdx;
public:

    
    virtual void init()
    {
        if (0 == m_vecObjs.size())
        {
            m_vecObjs.resize(OsnArrTools::s_nObjCountBegin);
        }
    }
    
    virtual ~OsnArrManager()
    {
        oUINT32 unSize = m_vecObjs.size();
        for (oUINT32 i = 0; i < unSize; ++i)
        {
            SAFE_DELETE(m_vecObjs[i]);
        }
        m_vecObjs.clear();
    }

    OsnArrManager()
    {
        m_vecObjs.resize(OsnArrTools::s_nObjCountBegin);
    }

    oUINT64 addObj(T *pObj)
    {
        oUINT64 unId = 0;
        if (NULL != pObj)
        {
            unId = createId();
            if (unId > 0)
            {
                addObject(unId, pObj);
                pObj->setId(unId);
            }
            else
            {
                SAFE_DELETE(pObj);
            }
        }
        return unId;
    }
    
    template<class OBJ_T>
    oUINT64 makeObj()
    {
        oUINT64 unId = createId();
        if (unId > 0)
        {
            T *pObj = new OBJ_T();
            addObject(unId, pObj);
            pObj->setId(unId);
            pObj->init();
        }
        return unId;
    }

    virtual oUINT64 createId()
    {
		oUINT64 unId = 0;
        lock();
        if (m_queFreeIds.size() > 0)
        {
            unId = m_queFreeIds.front();
            m_queFreeIds.pop();
        }
        else
        {
            unId = ++s_nObjIdx;
        }
        unlock();
        return unId;    
    }
    
    virtual void addObject(oUINT64 unId, T *pObj)
    {
        lock();
        oUINT64 u64Pos = OsnArrTools::getPos(unId);
        oUINT32 nCurCount = m_vecObjs.size();
        while (u64Pos >= nCurCount)
        {
            if (0 == nCurCount)
            {
                nCurCount = OsnArrTools::s_nObjCountBegin;
            }
            m_vecObjs.resize(nCurCount * 2);
            nCurCount = m_vecObjs.size();
        }
        m_vecObjs[u64Pos] = pObj;
        unlock();
    }
    
    virtual T* getObject(oUINT64 unId) const
    {
        T *pObj = NULL;
        lock();
        oUINT64 u64Pos = OsnArrTools::getPos(unId);
        if (u64Pos < m_vecObjs.size())
        {
            pObj = m_vecObjs[u64Pos];
        }
        unlock();
        return pObj;
    }
    
    virtual void removeObj(oUINT64 unId)
    {
        T *pObj = NULL;
        lock();
        oUINT64 u64Pos = OsnArrTools::getPos(unId);
        if (m_vecObjs.size() > u64Pos)
        {
            pObj = m_vecObjs[u64Pos];
            m_vecObjs[u64Pos] = NULL;
            oUINT64 u64Repeat = unId >> s_u64RepeatTimeMask;
            ++u64Repeat;
            unId = (u64Repeat << s_u64RepeatTimeMask) + u64Pos;
            m_queFreeIds.push(unId);
        }
        unlock();
        if (NULL != pObj)
        {
            pObj->exit();
            SAFE_DELETE(pObj);
        }
    }
protected:
    void lock() const
    {
        if (eThread_Saved == ThreadType)
        {
            m_Mutex.lock();
        }
    }
    
    void unlock() const
    {
        if (eThread_Saved == ThreadType)
        {
            m_Mutex.unlock();
        }
    }
protected:
    std::vector<T*> m_vecObjs;
    std::queue<oUINT64> m_queFreeIds;
    mutable OsnSpinLock m_Mutex;
};

template<class T, int ThreadType>
oINT32 OsnArrManager<T, ThreadType>::s_nObjIdx = 0;

#endif /* osn_arr_manager_hpp */
