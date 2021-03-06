//
//  osn_cache_arr_manager.h
//  osn
//
//  Created by zenghui on 17/4/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_cache_arr_manager_h
#define osn_cache_arr_manager_h
#include "osn_arr_manager.h"
#include <queue>

template<class T, int ThreadType = eThread_Normal>
class OsnCacheArrManager : public OsnArrManager<T, ThreadType>
{
public:
    OsnCacheArrManager()
        : m_unCachePercent(100)
    {
    }
    
    virtual ~OsnCacheArrManager()
    {
        while (m_queObjCache.size() > 0)
        {
            T *pObj = m_queObjCache.front();
            SAFE_DELETE(pObj);
            m_queObjCache.pop();
        }
    }
    
    void setCachePercent(oINT32 nPercent)
    {
        ATOM_SET(&m_unCachePercent, nPercent);
        if (m_unCachePercent <= 0 || m_unCachePercent > 100)
        {
            ATOM_SET(&m_unCachePercent, 100);
        }
    }

    template<class OBJ_T>
    oUINT64 makeObj()
    {
        oUINT64 unId = this->createId();
        if (unId > 0)
        {
            OBJ_T *pObj = NULL;
            this->lock();
            if (m_queObjCache.size() > 0)
            {
                pObj = m_queObjCache.front();
                m_queObjCache.pop();
            }
            this->unlock();
            if (NULL == pObj)
            {
                pObj = new OBJ_T();
            }
            this->addObject(unId, pObj);
            pObj->setId(unId);
            pObj->init();
        }
        return unId;
    }
    
    virtual void removeObj(oUINT64 nId)
    {
        this->lock();
        oUINT64 u64Pos = OsnArrTools::getPos(nId);
        if (this->m_vecObjs.size() > u64Pos)
        {
            T *pObj = this->m_vecObjs[u64Pos];
            pObj->exit();
            if (100 == m_unCachePercent || (this->m_vecObjs.size() > 0 && m_queObjCache.size() / this->m_vecObjs.size() < m_unCachePercent))
            {
                m_queObjCache.push(pObj);
            }
            this->m_vecObjs[u64Pos] = NULL;
            this->m_queFreeIds.push(nId);
        }
        this->unlock();
    }
private:
    std::queue<T*> m_queObjCache;
    oUINT32 m_unCachePercent;
};

#endif /* osn_cache_arr_manager_h */
