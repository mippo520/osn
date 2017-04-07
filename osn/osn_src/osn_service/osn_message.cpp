//
//  osn_message.cpp
//  osn
//
//  Created by liqing on 17/4/7.
//  Copyright © 2017年 liqing. All rights reserved.
//

#include "osn_message.h"

OsnMessage::OsnMessage()
{
    
}

OsnMessage::~OsnMessage()
{
    
}

void OsnMessage::setInt(oINT32 nValue)
{
    m_nValue = nValue;
}

oINT32 OsnMessage::getInt() const
{
    return m_nValue;
}


