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
    
    const static std::string s_strSocketErr;
public:
    OsnSocketChannel(const stChannelDesc &desc);
    ~OsnSocketChannel();
    
    void init();
    oBOOL connect(oBOOL bOnce);
private:
    oBOOL blockConnect(oBOOL bOnce);
    eConnectState checkConnect();
    std::string tryConnect(oBOOL bOnce);
    oBOOL connectOnce(std::string &errCode);
    oINT32 connectBackup();
    VOID_STMT_FUNC dispatchFunction();
    void closeChannelSocket();
    void dispatchBySession(const OsnPreparedStatement &stmt);
    void dispatchByOrder(const OsnPreparedStatement &stmt);
private:
    stChannelDesc m_Desc;
    OsnSocket m_Socket;
};

#endif /* osn_socket_channel_hpp */
