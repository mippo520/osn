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
    oUINT32 makeObj()
    {
        oUINT32 unId = createId();
        if (unId > 0) {
            OBJ_T *pObj = new OBJ_T();
            addObject(unId, pObj);
            pObj->setId(unId);
            pObj->init();
        }
        return unId;
    }

    virtual oUINT32 createId()
    {
		oUINT32 unId = 0;
        lock();
        if (m_queFreeIds.size() > 0) {
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
    
    virtual void addObject(oUINT32 unId, T *pObj)
    {
        lock();
        oUINT32 nCurCount = m_vecObjs.size();
        if (unId >= nCurCount) {
            if (0 == nCurCount) {
                nCurCount = s_nObjCountBegin;
            }
            m_vecObjs.resize(nCurCount * 2);
        }
        m_vecObjs[unId] = pObj;
        unlock();
    }
    
    virtual T* getObject(oUINT32 unId)
    {
        T *pObj = NULL;
        if (unId < m_vecObjs.size()) {
            pObj = m_vecObjs[unId];
        }
        return pObj;
    }
    
    virtual void removeObj(oUINT32 unId)
    {
        lock();
        if (m_vecObjs.size() > unId) {
            T *pObj = m_vecObjs[unId];
            pObj->exit();
            SAFE_DELETE(pObj);
            m_vecObjs[unId] = NULL;
            m_queFreeIds.push(unId);
        }
        unlock();
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
    std::queue<oUINT32> m_queFreeIds;
    std::mutex m_Mutex;
};

template<class T, int ThreadType>
oINT32 OsnArrManager<T, ThreadType>::s_nObjIdx = 0;

#endif /* osn_arr_manager_hpp */
