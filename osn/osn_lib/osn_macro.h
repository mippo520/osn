//
//  osn_macro.h
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_macro_h
#define osn_macro_h

#define SAFE_DELETE(x) if(NULL != x) { delete x; x = NULL; } 

#define MEMBER_VALUE(type, name)    \
    private:    \
        type m_##name;  \
    public: \
        const type& get##name() const    \
        { return m_##name; }    \
        void set##name(const type &value)   \
        { m_##name = value; }

#define ATOM_INC(ptr) __sync_add_and_fetch(ptr, 1)
#define ATOM_FINC(ptr) __sync_fetch_and_add(ptr, 1)
#define ATOM_DEC(ptr) __sync_sub_and_fetch(ptr, 1)
#define ATOM_FDEC(ptr) __sync_fetch_and_sub(ptr, 1)
#define ATOM_ADD(ptr,n) __sync_add_and_fetch(ptr, n)
#define ATOM_SUB(ptr,n) __sync_sub_and_fetch(ptr, n)
#define ATOM_SET(ptr,n) __sync_lock_test_and_set(ptr, n)

#endif /* osn_macro_h */
