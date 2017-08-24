//
//  osn_socket_channel.hpp
//  osn
//
//  Created by Mippo on 2017/8/24.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_socket_channel_hpp
#define osn_socket_channel_hpp
#include "osn_socket_head.h"
#include "osn_socket.h"

class OsnSocketChannel
{
    enum eConnectState
    {
        eCS_None = 0,
        eCS_True,
        eCS_False
    };
public:
    OsnSocketChannel(const stChannelDesc &desc);
    ~OsnSocketChannel();
    
    oBOOL connect(oBOOL bOnce);
private:
    oBOOL blockConnect(oBOOL bOnce);
    eConnectState checkConnect();
    oBOOL tryConnect(oBOOL bOnce);
    oBOOL connectOnce();
    oINT32 connectBackup();
private:
    stChannelDesc m_Desc;
    OsnSocket m_Socket;
};

#endif /* osn_socket_channel_hpp */
