//
//  osn_cache_arr_manager.h
//  osn
//
//  Created by liqing on 17/4/12.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_cache_arr_manager_h
#define osn_cache_arr_manager_h
#include "osn_arr_manager.h"
#include <queue>

template<class T, int ThreadType = eThread_Normal>
class OsnCacheArrManager : public OsnArrManager<T, ThreadType> {
public:
    OsnCacheArrManager()
        : m_nCachePercent(100)
    {
    }
    
    virtual ~OsnCacheArrManager()
    {
        while (m_queObjCache.size() > 0) {
            T *pObj = m_queObjCache.front();
            SAFE_DELETE(pObj);
            m_queObjCache.pop();
        }
    }
    
    void setCachePercent(oINT32 nPercent)
    {
        this->lock();
        m_nCachePercent = nPercent;
        if (m_nCachePercent <= 0 || m_nCachePercent > 100) {
            m_nCachePercent = 100;
        }
        this->unlock();
    }

    template<class OBJ_T>
    oINT32 makeObj()
    {
        oINT32 nId = this->createId();
        if (nId > 0) {
            OBJ_T *pObj = NULL;
            this->lock();
            if (m_queObjCache.size() > 0) {
                pObj = m_queObjCache.front();
                m_queObjCache.pop();
            }
            this->unlock();
            if (NULL == pObj)
            {
                pObj = new OBJ_T();
            }
            this->addObject(nId, pObj);
            pObj->setId(nId);
            pObj->init();
        }
        return nId;
    }
    
    virtual void removeObj(oINT32 nId)
    {
        this->lock();
        if (this->m_vecObjs.size() > nId) {
            T *pObj = this->m_vecObjs[nId];
            pObj->exit();
            if (100 == m_nCachePercent || (this->m_vecObjs.size() > 0 && m_queObjCache.size() / this->m_vecObjs.size() < m_nCachePercent))
            {
                m_queObjCache.push(pObj);
            }
            this->m_vecObjs[nId] = NULL;
            this->m_queFreeIds.push(nId);
        }
        this->unlock();
    }
private:
    std::queue<T*> m_queObjCache;
    oINT32 m_nCachePercent;
};

#endif /* osn_cache_arr_manager_h */
