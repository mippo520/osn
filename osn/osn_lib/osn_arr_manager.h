//
//  osn_arr_manager.hpp
//  osn
//
//  Created by liqing on 17/4/10.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_arr_manager_hpp
#define osn_arr_manager_hpp
#include <vector>
#include <queue>
#include <mutex>
#include "osn.h"

enum eThreadType
{
    eThread_Normal = 0,
    eThread_Saved,
};

template<class T, int ThreadType = eThread_Normal>
class OsnArrManager {
    const static oINT32 s_nObjCountBegin = 750;
    static oINT32 s_nObjIdx;
public:
    virtual void init()
    {
        if (0 == m_vecObjs.size()) {
            m_vecObjs.resize(s_nObjCountBegin);
        }
    }
    
    virtual ~OsnArrManager()
    {
        oUINT32 unSize = m_vecObjs.size();
        for (oUINT32 i = 0; i < unSize; ++i) {
            SAFE_DELETE(m_vecObjs[i]);
        }
        m_vecObjs.clear();
    }

    OsnArrManager()
    {
        m_vecObjs.resize(s_nObjCountBegin);
    }

    
    template<class OBJ_T>
    oINT32 makeObj()
    {
        oINT32 nId = createId();
        if (nId > 0) {
            OBJ_T *pObj = new OBJ_T();
            addObject(nId, pObj);
            pObj->setId(nId);
            pObj->init();
        }
        return nId;
    }

    virtual oINT32 createId()
    {
        oINT32 nId = 0;
        lock();
        if (m_queFreeIds.size() > 0) {
            nId = m_queFreeIds.front();
            m_queFreeIds.pop();
        }
        else
        {
            nId = ++s_nObjIdx;
        }
        unlock();
        return nId;    
    }
    
    virtual void addObject(oINT32 nId, T *pObj)
    {
        lock();
        oUINT32 nCurCount = m_vecObjs.size();
        if (nId >= nCurCount) {
            if (0 == nCurCount) {
                nCurCount = s_nObjCountBegin;
            }
            m_vecObjs.resize(nCurCount * 2);
        }
        m_vecObjs[nId] = pObj;
        unlock();
    }
    
    virtual T* getObject(oINT32 nId)
    {
        T *pObj = NULL;
        if (nId < m_vecObjs.size()) {
            pObj = m_vecObjs[nId];
        }
        return pObj;
    }
    
    virtual void removeObj(oINT32 nId)
    {
        if (m_vecObjs.size() > nId) {
            lock();
            T *pObj = m_vecObjs[nId];
            pObj->exit();
            SAFE_DELETE(pObj);
            m_vecObjs[nId] = NULL;
            m_queFreeIds.push(nId);
            unlock();
        }
    }
    
    void lock()
    {
        if (eThread_Saved == ThreadType) {
            m_Mutex.lock();
        }
    }
    
    void unlock()
    {
        if (eThread_Saved == ThreadType) {
            m_Mutex.unlock();
        }
    }
protected:
    std::vector<T*> m_vecObjs;
    std::queue<oINT32> m_queFreeIds;
    std::mutex m_Mutex;
};

template<class T, int ThreadType>
oINT32 OsnArrManager<T, ThreadType>::s_nObjIdx = 0;

#endif /* osn_arr_manager_hpp */
