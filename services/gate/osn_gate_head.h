//
//  osn_gate_head.h
//  Gate
//
//  Created by zenghui on 17/6/27.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_gate_head_h
#define osn_gate_head_h

namespace osn_gate
{
    enum eFuncType
    {
        Func_Open = 0,      // 开启socket
        Func_Close,         // 关闭socket
        Func_Kick,          // 关闭客户端
        Func_Forward,
        
        Func_Count
    };
}

#endif /* osn_gate_head_h */
