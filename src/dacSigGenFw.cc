#include "dacSigGenFw.h"

#include <cpsw_yaml.h>
#include <yaml-cpp/yaml.h>
#include <cpsw_sval.h>
#include <cpsw_entry_adapt.h>
#include <cpsw_hub.h>
#include <fstream>
#include <sstream>

#include <math.h>


#define CPSW_TRY_CATCH(X)       try {   \
        (X);                            \
    } catch (CPSWError &e) {            \
        fprintf(stderr,                 \
                "CPSW Error: %s at %s, line %d\n",     \
                e.getInfo().c_str(),    \
                __FILE__, __LINE__);    \
        throw e;                        \
    }

class CdacSigGenFwAdapt;
typedef shared_ptr<CdacSigGenFwAdapt> dacSigGenFwAdapt;

class CdacSigGenFwAdapt : public IdacSigGenFw, public IEntryAdapt {
private:
    Path pLlrfFeedbackWrapper_;
    Path pLlrfHls_;
//    Path pFeedbackWindow_;
    Path pIQWaveform_;

public:
    CdacSigGenFwAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie);


};


dacSigGenFw IdacSigGenFw::create(Path p)
{
    return IEntryAdapt::check_interface<dacSigGenFwAdapt, DevImpl>(p);
}

CdacSigGenFwAdapt::CdacSigGenFwAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie):
    IEntryAdapt(k, p, ie)

{
}

