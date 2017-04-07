//
//  osn_message.hpp
//  osn
//
//  Created by liqing on 17/4/7.
//  Copyright © 2017年 liqing. All rights reserved.
//

#ifndef osn_message_hpp
#define osn_message_hpp

#include "osn.h"

class OsnMessage {
public:
    OsnMessage();
    virtual ~OsnMessage();
    
    void setInt(oINT32 nValue);
    oINT32 getInt() const;
private:
    oINT32 m_nValue;
};

#endif /* osn_message_hpp */
