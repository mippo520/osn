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
    virtual oUINT32 getCurService() const = 0;
private:
    friend class OsnService;
    virtual void pushWarkingService(oUINT32 unId) const = 0;
    virtual oUINT32 sendMessage(oUINT32 unTargetId, oUINT32 unSource, oINT32 type, oUINT32 unSession, const OsnPreparedStatement *pMsg) const = 0;
};

extern const IOsnService *g_Service;

#endif /* I_osn_service_h */
