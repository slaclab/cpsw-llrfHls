#ifndef    _INTERLOCK_RTM_H
#define    _INTERLOCK_RTM_H

#include   <cpsw_api_user.h>
#include   <cpsw_api_builder.h>

class IinterlockRtmFw;
typedef shared_ptr<IinterlockRtmFw> interlockRtmFw;

class IinterlockRtmFw : public virtual IEntry {
public:
    static interlockRtmFw create(Path p);

    virtual void getRtmStatus(uint32_t *rtmStatus) = 0;
    virtual void getRtmFirmwareVersion(uint32_t *rtmFirmwareVersion) = 0;
    virtual void getRtmSystemId(char *systemIdString) = 0;
    virtual void getRtmSubType(char *subTypeString) = 0;
    virtual void getRtmFirmwareDate(char *firmwareDateString) = 0;
    virtual void setRtmKlyWiperRegA(uint32_t v) = 0;
    virtual void setRtmKlyWiperRegB(uint32_t v) = 0;
    virtual void setRtmKlyNVRegA(uint32_t v) = 0;
    virtual void setRtmKlyNVRegB(uint32_t v) = 0;
    virtual void setRtmModWiperRegA(uint32_t v) = 0;
    virtual void setRtmModWiperRegB(uint32_t v) = 0;
    virtual void setRtmModNVRegA(uint32_t v) = 0;
    virtual void setRtmModNVRegB(uint32_t v) = 0;
    virtual void setRtmCfgRegister(uint32_t v) = 0;
    virtual void getRtmFaultOutStatus(uint32_t *rtmFaultOutStatus) = 0;
    virtual void getRtmAdcLockedStatus(uint32_t *rtmAdcLockedStatus) = 0;
    virtual void getRtmRFOffStatus(uint32_t *rtmRFOffStatus) = 0;
    virtual void getRtmAdcIn(uint32_t v[]) = 0;
    virtual void setRtmMode(uint32_t v) = 0;
    virtual void setRtmDesiredSled(bool tune) = 0;
    virtual void getRtmFastAdcBufferBeamCurrentVoltage(uint32_t v[]) = 0;
    virtual void getRtmFastAdcBufferForwardReflect(uint32_t v[]) = 0;
    virtual void cmdRtmRearm(void) = 0;
    virtual void cmdRtmSwTrigger(void) = 0;
    virtual void cmdRtmClearFault(void) = 0;

    virtual void setFaultStreamEnable(uint32_t v) = 0;
    virtual void getFaultHistoryBuffer(uint32_t *writePointer, uint32_t timestamp[], uint32_t histIV[], uint32_t histFwdRef[]) = 0;

};


#endif /* _INTERLOCK_RTM_H */
