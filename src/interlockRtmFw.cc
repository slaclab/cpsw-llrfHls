#include "interlockRtmFw.h"
#include <cpsw_yaml.h>
#include <yaml-cpp/yaml.h>
#include <cpsw_sval.h>
#include <cpsw_entry_adapt.h>
#include <cpsw_hub.h>
#include <fstream>
#include <sstream>

class CinterlockRtmFwAdapt;
typedef shared_ptr<CinterlockRtmFwAdapt> interlockRtmFwAdapt;

class CinterlockRtmFwAdapt : public IinterlockRtmFw, public IEntryAdapt {

protected:
    Path         pCore_;
    ScalVal_RO   rtmStatus_;
    ScalVal_RO   rtmFirmwareVersion_;
    ScalVal_RO   rtmSystemId_;
    ScalVal_RO   rtmSubType_;
    ScalVal_RO   rtmFirmwareDate_;
    ScalVal      rtmKlyWiperRegA_;
    ScalVal      rtmKlyWiperRegB_;
    ScalVal      rtmKlyNVRegA_;
    ScalVal      rtmKlyNVRegB_;
    ScalVal      rtmModWiperRegA_;
    ScalVal      rtmModWiperRegB_;
    ScalVal      rtmModNVRegA_;
    ScalVal      rtmModNVRegB_;
    ScalVal      rtmCfgRegister_;
    ScalVal_RO   rtmFaultOutStatus_;
    ScalVal_RO   rtmAdcLockedStatus_;
    ScalVal_RO   rtmRFOffStatus_;
    ScalVal_RO   rtmAdcIn_;
    ScalVal      rtmMode_;
    ScalVal      rtmTuneSled_;
    ScalVal      rtmDetuneSled_;
    ScalVal_RO   rtmAdcBufferBeamIV_;
    ScalVal_RO   rtmAdcBufferFwdRef_;
    Command      rtmRearm_Cmd_;
    Command      rtmSwTrigger_Cmd_;
    Command      rtmClearFault_Cmd_;

    ScalVal      rtmFaultStreamEnable_;
    ScalVal_RO   rtmFaultBufferWritePointer_;
    ScalVal_RO   rtmFaultTimeStampBuffer_;
    ScalVal_RO   rtmAdcHistoryBufferBeamIV_;
    ScalVal_RO   rtmAdcHistoryBufferFwdRef_;


public:
    CinterlockRtmFwAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie);

    virtual void getRtmStatus(uint32_t *rtmStatus);
    virtual void getRtmFirmwareVersion(uint32_t *rtmFirmwareVersion);
    virtual void getRtmSystemId(char *systemIdString);
    virtual void getRtmSubType(char *subTypeString);
    virtual void getRtmFirmwareDate(char *firmwareDateString);
    virtual void setRtmKlyWiperRegA(uint32_t v);
    virtual void setRtmKlyWiperRegB(uint32_t v);
    virtual void setRtmKlyNVRegA(uint32_t v);
    virtual void setRtmKlyNVRegB(uint32_t v);
    virtual void setRtmModWiperRegA(uint32_t v);
    virtual void setRtmModWiperRegB(uint32_t v);
    virtual void setRtmModNVRegA(uint32_t v);
    virtual void setRtmModNVRegB(uint32_t v);
    virtual void setRtmCfgRegister(uint32_t v);
    virtual void getRtmFaultOutStatus(uint32_t *rtmFaultOutStatus);
    virtual void getRtmAdcLockedStatus(uint32_t *rtmAdcLockedStatus);
    virtual void getRtmRFOffStatus(uint32_t *rtmRFOffStatus);
    virtual void getRtmAdcIn(uint32_t v[]);
    virtual void setRtmMode(uint32_t v);
    virtual void setRtmDesiredSled(bool tune);
    virtual void getRtmFastAdcBufferBeamCurrentVoltage(uint32_t v[]);
    virtual void getRtmFastAdcBufferForwardReflect(uint32_t v[]);
    virtual void cmdRtmRearm(void);
    virtual void cmdRtmSwTrigger(void);
    virtual void cmdRtmClearFault(void);

    virtual void setFaultStreamEnable(uint32_t v);
    virtual void getFaultHistoryBuffer(uint32_t *writePointer, uint32_t timestamp[], uint32_t histIV[], uint32_t histFwdRef[]);
};





interlockRtmFw IinterlockRtmFw::create(Path p)
{
    return IEntryAdapt::check_interface<interlockRtmFwAdapt, DevImpl>(p);

}

CinterlockRtmFwAdapt::CinterlockRtmFwAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie):
        IEntryAdapt(k, p, ie),
  pCore_(                p->findByName("AppTop/AppCore") ),
  rtmStatus_(            IScalVal_RO::create( pCore_->findByName("RtmRfInterlock/Status") ) ),
  rtmFirmwareVersion_(   IScalVal_RO::create( pCore_->findByName("RtmRfInterlock/FirmwareVersion") ) ),
  rtmSystemId_(          IScalVal_RO::create( pCore_->findByName("RtmRfInterlock/SystemId") ) ),
  rtmSubType_(           IScalVal_RO::create( pCore_->findByName("RtmRfInterlock/SubType") ) ),
  rtmFirmwareDate_(      IScalVal_RO::create( pCore_->findByName("RtmRfInterlock/FirmwareDate") ) ),
  rtmKlyWiperRegA_(      IScalVal::create(pCore_->findByName("RtmRfInterlock/KlyWiperRegA") ) ),
  rtmKlyWiperRegB_(      IScalVal::create(pCore_->findByName("RtmRfInterlock/KlyWiperRegB") ) ),
  rtmKlyNVRegA_(         IScalVal::create(pCore_->findByName("RtmRfInterlock/KlyNVRegA") ) ),
  rtmKlyNVRegB_(         IScalVal::create(pCore_->findByName("RtmRfInterlock/KlyNVRegB") ) ),
  rtmModWiperRegA_(      IScalVal::create(pCore_->findByName("RtmRfInterlock/ModWiperRegA") ) ),
  rtmModWiperRegB_(      IScalVal::create(pCore_->findByName("RtmRfInterlock/ModWiperRegB") ) ),
  rtmModNVRegA_(         IScalVal::create(pCore_->findByName("RtmRfInterlock/ModNVRegA") ) ),
  rtmModNVRegB_(         IScalVal::create(pCore_->findByName("RtmRfInterlock/ModNVRegB") ) ),
  rtmCfgRegister_(       IScalVal::create(pCore_->findByName("RtmRfInterlock/CfgRegister") ) ),
  rtmFaultOutStatus_(    IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/FaultOut") ) ),
  rtmAdcLockedStatus_(   IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/AdcLocked") ) ),
  rtmRFOffStatus_(       IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/RfOff") ) ),
  rtmAdcIn_(             IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/AdcIn") ) ),
  rtmMode_(              IScalVal::create(pCore_->findByName("RtmRfInterlock/Mode") ) ),
  rtmTuneSled_(          IScalVal::create(pCore_->findByName("RtmRfInterlock/TuneSled") ) ),
  rtmDetuneSled_(        IScalVal::create(pCore_->findByName("RtmRfInterlock/DetuneSled") ) ),
  rtmAdcBufferBeamIV_(   IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/RtmAdcBuffer[0]/MemoryArray") ) ),
  rtmAdcBufferFwdRef_(   IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/RtmAdcBuffer[1]/MemoryArray") ) ),
  rtmRearm_Cmd_(         ICommand::create(pCore_->findByName("RtmRfInterlock/RearmTrigger") ) ),
  rtmSwTrigger_Cmd_(     ICommand::create(pCore_->findByName("RtmRfInterlock/SwTrigger") ) ),
  rtmClearFault_Cmd_(    ICommand::create(pCore_->findByName("RtmRfInterlock/ClearFault") ) ),

  rtmFaultStreamEnable_(      IScalVal   ::create(pCore_->findByName("RtmRfInterlock/StreamEnable") ) ),
  rtmFaultBufferWritePointer_(IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/WritePointer") ) ),
  rtmFaultTimeStampBuffer_   (IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/timestampBuffer") ) ),
  rtmAdcHistoryBufferBeamIV_( IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/RtmAdcHistoryBuffer[0]/MemoryArray") ) ),
  rtmAdcHistoryBufferFwdRef_( IScalVal_RO::create(pCore_->findByName("RtmRfInterlock/RtmAdcHistoryBuffer[1]/MemoryArray") ) )
{
}




void CinterlockRtmFwAdapt::getRtmStatus(uint32_t *rtmStatus)
{
    try {
        rtmStatus_->getVal(rtmStatus);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}


void CinterlockRtmFwAdapt::getRtmFirmwareVersion(uint32_t *rtmFirmwareVersion)
{
    try {
        rtmFirmwareVersion_->getVal(rtmFirmwareVersion);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}


void CinterlockRtmFwAdapt::getRtmSystemId(char *systemIdString)
{
    uint16_t id[4];
    char *c;
    int i;

    try {
        rtmSystemId_->getVal(id, 4);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }

    c = (char*) id;

    for(i=0;i<4; i++) {   // swapped byte order for string
        *(systemIdString +(i*2)) = *(c+(i*2+1));
        *(systemIdString +(i*2+1)) = *(c+(i*2));
    }

    *(systemIdString + (i*2)) = '\0';
    
}


void CinterlockRtmFwAdapt::getRtmSubType(char *subTypeString)
{
    uint16_t stype[4];
    char *c;
    int i;

    try {
        rtmSubType_->getVal(stype, 4);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }

    c = (char *) stype;

    for(i=0; i<4; i++) {    // swapped byte order for string
        *(subTypeString+(i*2)) = *(c+(i*2+1));
        *(subTypeString+(i*2+1)) = *(c+(i*2));
    }
    *(subTypeString +(i*2)) = '\0';
}


void CinterlockRtmFwAdapt::getRtmFirmwareDate(char *firmwareDateString)
{
    uint16_t sdate[4];
    char *c;
    int i;
    
    try {
        rtmFirmwareDate_->getVal(sdate,4);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }

    c = (char*) sdate;

    for(i=0; i<4; i++) {    // swapped byte order for string
        *(firmwareDateString + (i*2)) = *(c+(i*2+1));
        *(firmwareDateString + (i*2+1)) = *(c+(i*2));
    }

    *(firmwareDateString+(i*2)) = '\0';

}

void CinterlockRtmFwAdapt::setRtmKlyWiperRegA(uint32_t reg)
{
    try {
        rtmKlyWiperRegA_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmKlyWiperRegB(uint32_t reg)
{
    try {
        rtmKlyWiperRegB_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmKlyNVRegA(uint32_t reg)
{

    try {
        rtmKlyNVRegA_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmKlyNVRegB(uint32_t reg)
{
    try {
        rtmKlyNVRegB_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmModWiperRegA(uint32_t reg)
{
    try {
        rtmModWiperRegA_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmModWiperRegB(uint32_t reg)
{
    try {
        rtmModWiperRegB_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmModNVRegA(uint32_t reg)
{
    try {
        rtmModNVRegA_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmModNVRegB(uint32_t reg)
{
    try {
        rtmModNVRegB_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmCfgRegister(uint32_t reg)
{
    try {
        rtmCfgRegister_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::getRtmFaultOutStatus(uint32_t *rtmFaultOutStatus)
{
    try {
        rtmFaultOutStatus_->getVal(rtmFaultOutStatus);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::getRtmAdcLockedStatus(uint32_t *rtmAdcLockedStatus)
{    
    try {
        rtmAdcLockedStatus_->getVal(rtmAdcLockedStatus);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::getRtmRFOffStatus(uint32_t *rtmRFOffStatus)
{
    try {
        rtmRFOffStatus_->getVal(rtmRFOffStatus);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::getRtmAdcIn(uint32_t v[])
{
    try {
        rtmAdcIn_->getVal(v,4);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmMode(uint32_t reg)
{
    try {
        rtmMode_->setVal(reg);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setRtmDesiredSled(bool tune)
{
    try {
        if(tune) {
            rtmDetuneSled_->setVal( (uint64_t) 0);
            rtmTuneSled_->setVal( (uint64_t) 1);
        }
        else {
            rtmTuneSled_->setVal( (uint64_t) 0);
            rtmDetuneSled_->setVal( (uint64_t) 1);
        }
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::getRtmFastAdcBufferBeamCurrentVoltage(uint32_t v[])
{
    try {
        rtmAdcBufferBeamIV_->getVal(v, (unsigned) 0x200);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::getRtmFastAdcBufferForwardReflect(uint32_t v[])
{
    try {
        rtmAdcBufferFwdRef_->getVal(v, (unsigned) 0x200);
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::cmdRtmRearm(void)
{
    try {
        rtmRearm_Cmd_->execute();
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::cmdRtmSwTrigger(void)
{
    try {
        rtmSwTrigger_Cmd_->execute();
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::cmdRtmClearFault(void)
{
    try {
        rtmClearFault_Cmd_->execute();
    } catch( CPSWError &e ) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}

void CinterlockRtmFwAdapt::setFaultStreamEnable(uint32_t v)
{
    try {
        rtmFaultStreamEnable_->setVal((v)?1:0);
    } catch (CPSWError &e) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}


void CinterlockRtmFwAdapt::getFaultHistoryBuffer(uint32_t *writePointer, uint32_t timestamp[], uint32_t histIV[], uint32_t histFwdRef[])
{
    try {
        rtmFaultBufferWritePointer_->getVal(writePointer, 0x1);
        rtmFaultTimeStampBuffer_->getVal(timestamp, 0x8);
        rtmAdcHistoryBufferBeamIV_->getVal(histIV, 0x800);
        rtmAdcHistoryBufferFwdRef_->getVal(histFwdRef, 0x800);
    } catch (CPSWError &e) {
        fprintf(stderr,"CPSW Error: %s\n", e.getInfo().c_str());
        throw e;
    }
}





