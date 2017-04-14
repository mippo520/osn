//
//  osn_macro.h
//  osn
//
//  Created by liqing on 17/4/5.
//  Copyright © 2017年 liqing. All rights reserved.
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

#endif /* osn_macro_h */
