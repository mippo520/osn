//
//  I_osn_service.h
//  osn
//
//  Created by zenghui on 17/5/12.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef I_osn_service_h
#define I_osn_service_h

class IOsnService
{
public:
    virtual ID_SERVICE getCurService() const = 0;
private:
    friend class OsnService;
    virtual void pushWarkingService(ID_SERVICE unId) const = 0;
    virtual ID_SESSION sendMessage(ID_SERVICE unTargetId, ID_SERVICE unSource, oINT32 type, ID_SESSION unSession, const OsnPreparedStatement &msg = OsnPreparedStatement()) const = 0;
};

extern const IOsnService *g_Service;

#endif /* I_osn_service_h */
