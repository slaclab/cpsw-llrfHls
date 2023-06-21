#include "llrfDestnTrig.h"

#include <cpsw_yaml.h>
#include <yaml-cpp/yaml.h>
#include <cpsw_sval.h>
#include <cpsw_entry_adapt.h>
#include <cpsw_hub.h>
#include <fstream>
#include <sstream>



#define CPSW_TRY_CATCH(X)       try {   \
        (X);                            \
    } catch (CPSWError &e) {            \
        fprintf(stderr,                 \
                "CPSW Error: %s at %s, line %d\n",     \
                e.getInfo().c_str(),    \
                __FILE__, __LINE__);    \
        throw e;                        \
    }


class CllrfDestnTrigAdapt;
typedef shared_ptr<CllrfDestnTrigAdapt> llrfDestnTrigAdapt;

class CllrfDestnTrigAdapt : public IllrfDestnTrig, public IEntryAdapt {

protected:
    Path pAppCore;
    Path pDestn[MAX_NUM_DEST];
    struct Destn_st {
        ScalVal    enable;
        ScalVal    source;
        ScalVal    polarity;
        ScalVal    delay;
        ScalVal    width;
    }  *pDestn_st[MAX_NUM_DEST];
public:
    CllrfDestnTrigAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie);

    virtual void setEnable(int index, uint32_t enable);
    virtual void setSource(int index, uint32_t source);
    virtual void setPolarity(int index, uint32_t polarity);
    virtual void setDelay(int index, uint32_t delay);
    virtual void setWidth(int index, uint32_t width);

};


llrfDestnTrig IllrfDestnTrig::create(Path p)
{
    return IEntryAdapt::check_interface<llrfDestnTrigAdapt, DevImpl>(p);
}

CllrfDestnTrigAdapt::CllrfDestnTrigAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie):
    IEntryAdapt(k, p, ie),
    pAppCore(p->findByName(""))
{
    char str_name[80];

    for(int i=0; i<MAX_NUM_DEST; i++) {
        pDestn_st[i] = new Destn_st;
        sprintf(str_name, "DestnTrig[%d]", i); pDestn[i] = pAppCore->findByName(str_name);
        pDestn_st[i]->enable    = IScalVal::create(pDestn[i]->findByName("Enable"));
        pDestn_st[i]->source    = IScalVal::create(pDestn[i]->findByName("Source"));
        pDestn_st[i]->polarity  = IScalVal::create(pDestn[i]->findByName("Polarity"));
        pDestn_st[i]->delay     = IScalVal::create(pDestn[i]->findByName("Delay"));
        pDestn_st[i]->width     = IScalVal::create(pDestn[i]->findByName("Width"));

       setSource(i, 0);    // hard code, default source
    }
}

void CllrfDestnTrigAdapt::setEnable(int index, uint32_t enable)
{
    CPSW_TRY_CATCH(pDestn_st[index]->enable->setVal(enable?1:0));
}

void CllrfDestnTrigAdapt::setSource(int index, uint32_t source)
{
    CPSW_TRY_CATCH(pDestn_st[index]->source->setVal(source));
}

void CllrfDestnTrigAdapt::setPolarity(int index, uint32_t polarity)
{
    CPSW_TRY_CATCH(pDestn_st[index]->polarity->setVal(polarity?1:0));
}

void CllrfDestnTrigAdapt::setDelay(int index, uint32_t delay)
{
    CPSW_TRY_CATCH(pDestn_st[index]->delay->setVal(delay));
}

void CllrfDestnTrigAdapt::setWidth(int index, uint32_t width)
{
    CPSW_TRY_CATCH(pDestn_st[index]->width->setVal(width));
}

