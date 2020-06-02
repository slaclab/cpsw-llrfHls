#include "llrfFw.h"

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


class CllrfFwAdapt;
typedef shared_ptr<CllrfFwAdapt> llrfFwAdapt;

class CllrfFwAdapt : public IllrfFw, public IEntryAdapt {
private:
    double ref_weight_input[NUM_CH], ref_weight_norm[NUM_CH];
    double fb_weight_input[NUM_CH],  fb_weight_norm[NUM_CH];

protected:
    Path pLlrfFeedbackWrapper_;
    Path pLlrfHls_;
    Path pFeedbackWindow_;
    Path pIQWaveform_;

    /* put ScalVals, etc. here */
    DoubleVal p_ref_offset_;              // phase offset for reference
    DoubleVal p_fb_offset_;               // phase offset for feedback
    DoubleVal p_gain_;                    // gain for phase loop
    DoubleVal a_gain_;                    // gain for amplitude loop

    ScalVal ref_subtraction_enable_;    // enable/disable for reference subtraction
    ScalVal fb_phase_enable_;           // enable/disable for phase loop
    ScalVal fb_ampl_enable_;            // enable/disable for amplitude loop

    DoubleVal p_corr_upper_;              // upper limit for phase correction
    DoubleVal p_corr_lower_;              // lower lomit for phase correction
    DoubleVal a_corr_upper_;              // upper limit for amplitude correction
    DoubleVal a_corr_lower_;              // lower limit for amplitude correction
    DoubleVal a_drv_upper_;               // upper drive limit for amplitude
    DoubleVal a_drv_lower_;               // lower drive limit for amplitude
    DoubleVal ref_weight_;                // channel weight for reference average, for each channel, array[10]
    DoubleVal fb_weight_;                 // channel weight for feedback average, for each channel, array[10]
    DoubleVal p_offset_[NUM_CH];                  // phase offset for each channel, array[10]
    DoubleVal p_des_[NUM_TIMESLOT];                     // desired phase, PDES, for each timeslot, array[18]
    DoubleVal a_des_[NUM_TIMESLOT];                     // desired amplitude, ADES, for each timeslot, array[18]

    DoubleVal_RO p_ch_;                      // window averaged phase for each channel, array[10]
    DoubleVal_RO a_ch_;                      // window averaged amplitude for each channel, array[10]
    DoubleVal_RO p_fb_;                      // channel averaged phase for each timeslot, array[18]
    DoubleVal_RO a_fb_;                      // channel averaged amplitude for each timeslot, array[18]
    DoubleVal_RO p_ref_;                     // channel averaged phase for reference, for each timeslot, array[18]
    DoubleVal_RO a_ref_;                     // channel averaged amplitude for reference, for each timeslot, array[18]

    DoubleVal_RO p_set_;                  // phase set value for each timeslot, array[18]
    DoubleVal_RO a_set_;                  // ampliktude set value for each timeslot, array[18]

    ScalVal   avg_window_;                // average window
    ScalVal   i_waveform_ch_[NUM_CH];     // i waveform for each channel
    ScalVal   q_waveform_ch_[NUM_CH];     // q waveform for each channel 

public:
    CllrfFwAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie);

    virtual void setPhaseReferenceOffset(double phase);
    virtual void setPhaseFeedbackOffset(double phase);
    virtual void setPhaseGain(double gain);
    virtual void setAmplGain(double gain);

    virtual void setReferenceSubtractionEnable(bool enable);
    virtual void setPhaseFeedbackEnable(bool enable);
    virtual void setAmplFeedbackEnable(bool enable);

    virtual void setPhaseCorrectionUpperLimit(double limit);
    virtual void setPhaseCorrectionLowerLimit(double limit);
    virtual void setAmplCorrectionUpperLimit(double limit);
    virtual void setAmplCorrectionLowerLimit(double limit);
    virtual void setAmplDriveUpperLimit(double limit);
    virtual void setAmplDriveLowerLimit(double limit);
    virtual void setReferenceChannelWeight(double weight, int channel);
    virtual void setFeedbackChannelWeight(double weight, int channel);
    virtual void setPhaseOffset(double phase, int channel);
    virtual void setDesiredPhase(double phase, int timeslot);
    virtual void setDesiredAmpl(double ampl, int timeslot);

    virtual void getPhaseAllChannels(double *phase);
    virtual void getAmplAllChannels(double *ampl);
    virtual void getFeedbackPhaseAllTimeslots(double *phase);
    virtual void getFeedbackAmplAllTimeslots(double *ampl);
    virtual void getReferencePhaseAllTimeslots(double *phase);
    virtual void getReferenceAmplAllTimeslots(double *ampl);
    virtual void getPhaseSetAllTimeslots(double *phase);
    virtual void getAmplSetAllTimeslots(double *ampl);

    virtual void setAverageWindow(double *window);
    virtual void getIWaveform(double *i_waveform, int channel);
    virtual void getQWaveform(double *q_waveform, int channel);
    

};


llrfFw IllrfFw::create(Path p)
{
    return IEntryAdapt::check_interface<llrfFwAdapt, DevImpl>(p);
}


CllrfFwAdapt::CllrfFwAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie):
    IEntryAdapt(k, p, ie),
    pLlrfFeedbackWrapper_(p->findByName("AppTop/AppCore/LlrfFeedbackWrapper")),
    pLlrfHls_(       pLlrfFeedbackWrapper_->findByName("LlrfHls")),
    pFeedbackWindow_(pLlrfFeedbackWrapper_->findByName("FeedbackWindow")),
    pIQWaveform_(    pLlrfFeedbackWrapper_->findByName("IQWaveform")),

    p_ref_offset_(   IDoubleVal::create(pLlrfHls_->findByName("P_REF_OFFSET"))),
    p_fb_offset_(    IDoubleVal::create(pLlrfHls_->findByName("P_FB_OFFSET"))),
    p_gain_(         IDoubleVal::create(pLlrfHls_->findByName("P_GAIN"))),
    a_gain_(         IDoubleVal::create(pLlrfHls_->findByName("A_GAIN"))),

    ref_subtraction_enable_(IScalVal::create(pLlrfHls_->findByName("REF_SUBTRACTION_ENABLE"))),
    fb_phase_enable_(       IScalVal::create(pLlrfHls_->findByName("FB_PHASE_ENABLE"))),
    fb_ampl_enable_(        IScalVal::create(pLlrfHls_->findByName("FB_AMPL_ENABLE"))),

    p_corr_upper_(          IDoubleVal::create(pLlrfHls_->findByName("P_CORR_UPPER"))),
    p_corr_lower_(          IDoubleVal::create(pLlrfHls_->findByName("P_CORR_LOWER"))),
    a_corr_upper_(          IDoubleVal::create(pLlrfHls_->findByName("A_CORR_UPPER"))),
    a_corr_lower_(          IDoubleVal::create(pLlrfHls_->findByName("A_CORR_LOWER"))),
    a_drv_upper_(           IDoubleVal::create(pLlrfHls_->findByName("A_DRV_UPPER"))),
    a_drv_lower_(           IDoubleVal::create(pLlrfHls_->findByName("A_DRV_LOWER"))),
    ref_weight_(            IDoubleVal::create(pLlrfHls_->findByName("REF_WEIGHT"))),  
    fb_weight_ (            IDoubleVal::create(pLlrfHls_->findByName("FB_WEIGHT"))),
    // p_offset_     // make multiple instances in function body, 10 instances
    // p_des_        // make multiple instances in function body, 18 instances
    // a_des_        // make multiple instances in function body, 18 instances

    p_ch_(                  IDoubleVal_RO::create(pLlrfHls_->findByName("P_CH"))),    // array[10], get all of channel data at once
    a_ch_(                  IDoubleVal_RO::create(pLlrfHls_->findByName("A_CH"))),    // array[10], get all of channel data at onced
    p_fb_(                  IDoubleVal_RO::create(pLlrfHls_->findByName("P_FB"))),    // array[18], get all of timeslot data at once
    a_fb_(                  IDoubleVal_RO::create(pLlrfHls_->findByName("A_FB"))),    // array[18], get all of timeslot data at once
    p_ref_(                 IDoubleVal_RO::create(pLlrfHls_->findByName("P_REF"))),   // array[18], get all of timeslot data at once
    a_ref_(                 IDoubleVal_RO::create(pLlrfHls_->findByName("A_REF"))),   // array[18], get all of timeslot data at once

    p_set_(                 IDoubleVal_RO::create(pLlrfHls_->findByName("P_SET"))),   // array[18], get all of timeslot data at once
    a_set_(                 IDoubleVal_RO::create(pLlrfHls_->findByName("A_SET"))),   // array[18], get all of timeslot data at once

    avg_window_(            IScalVal::create(pFeedbackWindow_->findByName("Window")))  // array[4096], average window

{
    char str_name[80];

    for(int i = 0; i < NUM_CH; i++) {
        sprintf(str_name, "P_OFFSET[%d]",   i);           p_offset_[i]      = IDoubleVal::create(pLlrfHls_->findByName(str_name));
        sprintf(str_name, "IQWaveform[%d]/waveformI", i); i_waveform_ch_[i] = IScalVal::create(pLlrfHls_->findByName(str_name));
        sprintf(str_name, "IQWaveform[%d]/waveformQ", i); q_waveform_ch_[i] = IScalVal::create(pLlrfHls_->findByName(str_name));

        ref_weight_input[i] = ref_weight_norm[i] = 0.;    // intitialize the referecne channel weight
        fb_weight_input[i]  = fb_weight_norm[i]  = 0.;    // iniitialize the feeedback channel weight
    }

    for(int i = 0; i < NUM_TIMESLOT; i++) {
        sprintf(str_name, "P_DES[%d]", i); p_des_[i] = IDoubleVal::create(pLlrfHls_->findByName(str_name));
        sprintf(str_name, "A_DES[%d]", i); a_des_[i] = IDoubleVal::create(pLlrfHls_->findByName(str_name));
    }    
}


void CllrfFwAdapt::setPhaseReferenceOffset(double phase)
{
    phase *= (M_PI/180.);    // degree to radian

    CPSW_TRY_CATCH(p_ref_offset_->setVal(phase));
}

void CllrfFwAdapt::setPhaseFeedbackOffset(double phase)
{
    phase *= (M_PI/180.);     // degree to radian

    CPSW_TRY_CATCH(p_fb_offset_->setVal(phase));
}

void CllrfFwAdapt::setPhaseGain(double gain)
{
    CPSW_TRY_CATCH(p_gain_->setVal(gain));
}

void CllrfFwAdapt::setAmplGain(double gain)
{
    CPSW_TRY_CATCH(a_gain_->setVal(gain));
}

void CllrfFwAdapt::setReferenceSubtractionEnable(bool enable)
{
    CPSW_TRY_CATCH(ref_subtraction_enable_->setVal(enable?1:0));
}

void CllrfFwAdapt::setPhaseFeedbackEnable(bool enable)
{
    CPSW_TRY_CATCH(fb_phase_enable_->setVal(enable?1:0));
}

void CllrfFwAdapt::setAmplFeedbackEnable(bool enable)
{
    CPSW_TRY_CATCH(fb_ampl_enable_->setVal(enable?1:0));
}


void CllrfFwAdapt::setPhaseCorrectionUpperLimit(double limit)
{
    limit *= (M_PI/180.);    // degree to radian
    CPSW_TRY_CATCH(p_corr_upper_->setVal(limit));
}

void CllrfFwAdapt::setPhaseCorrectionLowerLimit(double limit)
{
    limit *= (M_PI/180.);    // degree to radian
    CPSW_TRY_CATCH(p_corr_lower_->setVal(limit));
}

void CllrfFwAdapt::setAmplCorrectionUpperLimit(double limit)
{
    CPSW_TRY_CATCH(a_corr_upper_->setVal(limit));
}

void CllrfFwAdapt::setAmplCorrectionLowerLimit(double limit)
{
    CPSW_TRY_CATCH(a_corr_lower_->setVal(limit));
}

void CllrfFwAdapt::setAmplDriveUpperLimit(double limit)
{
    CPSW_TRY_CATCH(a_drv_upper_->setVal(limit));
}

void CllrfFwAdapt::setAmplDriveLowerLimit(double limit)
{
    CPSW_TRY_CATCH(a_drv_lower_->setVal(limit));
}

void CllrfFwAdapt::setReferenceChannelWeight(double weight, int channel)
{
    double sum = 0.;
    for(int i = 0; i < NUM_CH; i++) {
        sum += ref_weight_input[i];
    }
    if(sum <0.) return;

    for(int i = 0; i < NUM_CH; i++) {
        ref_weight_norm[i] /= sum;
    }

    CPSW_TRY_CATCH(ref_weight_->setVal(ref_weight_norm, NUM_CH));
}

void CllrfFwAdapt::setFeedbackChannelWeight(double weight, int channel)
{
    double sum = 0.;
    for(int i = 0; i < NUM_CH; i++) {
        sum += fb_weight_input[i];
    }
    if(sum < 0.) return;

    for(int i = 0; i < NUM_CH; i++) {
        fb_weight_norm[i] /= sum;
    }

    CPSW_TRY_CATCH(fb_weight_->setVal(fb_weight_norm, NUM_CH));
}

void CllrfFwAdapt::setPhaseOffset(double offset, int channel)
{
    offset *= (M_PI/180.);    // degree to radian
    CPSW_TRY_CATCH(p_offset_[channel]->setVal(offset));
}

void CllrfFwAdapt::setDesiredPhase(double phase, int timeslot)
{
    phase *= (M_PI/180.);    // degree to radian
    CPSW_TRY_CATCH(p_des_[timeslot]->setVal(phase));
}

void CllrfFwAdapt::setDesiredAmpl(double ampl, int timeslot)
{
    CPSW_TRY_CATCH(a_des_[timeslot]->setVal(ampl));
}

void CllrfFwAdapt::getPhaseAllChannels(double *phase)
{
    CPSW_TRY_CATCH(p_ch_->getVal(phase, NUM_CH));

    for(int i = 0; i < NUM_CH; i++) {
        *(phase + i) *= (180./M_PI);    // radian to degree
    }
}

void CllrfFwAdapt::getAmplAllChannels(double *ampl)
{
    CPSW_TRY_CATCH(a_ch_->getVal(ampl, NUM_CH));
}


void CllrfFwAdapt::getFeedbackPhaseAllTimeslots(double *phase)
{
    CPSW_TRY_CATCH(p_fb_->getVal(phase, NUM_TIMESLOT));

    for(int i = 0; i < NUM_TIMESLOT; i++) {
        *(phase + i) *= (180./M_PI);    // radian to degree
    }
}

void CllrfFwAdapt::getFeedbackAmplAllTimeslots(double *ampl)
{
    CPSW_TRY_CATCH(a_fb_->getVal(ampl, NUM_TIMESLOT));
}

void CllrfFwAdapt::getReferencePhaseAllTimeslots(double *phase)
{
    CPSW_TRY_CATCH(p_ref_->getVal(phase, NUM_TIMESLOT));

    for(int i = 0; i < NUM_TIMESLOT; i++) {
        *(phase + i) *= (180./M_PI);    // radian to degree
    }
}

void CllrfFwAdapt::getReferenceAmplAllTimeslots(double *ampl)
{
    CPSW_TRY_CATCH(a_ref_->getVal(ampl, NUM_TIMESLOT));
}

void CllrfFwAdapt::getPhaseSetAllTimeslots(double *phase)
{
    CPSW_TRY_CATCH(p_set_->getVal(phase, NUM_TIMESLOT));

    for(int i = 0; i < NUM_TIMESLOT; i++) {
        *(phase + i) *= (180./M_PI);    // radian to degree
    }
}

void CllrfFwAdapt::getAmplSetAllTimeslots(double *ampl)
{
    CPSW_TRY_CATCH(a_set_->getVal(ampl, NUM_TIMESLOT));
}


void CllrfFwAdapt::setAverageWindow(double *window)
{
    int16_t window_out[MAX_SAMPLES];

    for(int i = 0; i < MAX_SAMPLES; i++) {
        window_out[i] = (int16_t)(*(window + i) * 0x7fff);
    }

    CPSW_TRY_CATCH(avg_window_->setVal((uint16_t*) window_out, MAX_SAMPLES));
}


void CllrfFwAdapt::getIWaveform(double *i_waveform, int channel)
{
    int16_t waveform[MAX_SAMPLES];
    CPSW_TRY_CATCH(i_waveform_ch_[channel]->getVal((uint16_t*)waveform, MAX_SAMPLES));

    for(int i = 0; i < MAX_SAMPLES; i++) {
        *(i_waveform + i) = (double)(waveform[i])/(double)(0x7fff);
    }
}

void CllrfFwAdapt::getQWaveform(double *q_waveform, int channel)
{
    int16_t waveform[MAX_SAMPLES];
    CPSW_TRY_CATCH(q_waveform_ch_[channel]->getVal((uint16_t*)waveform, MAX_SAMPLES));


    for(int i = 0; i < MAX_SAMPLES; i++) {
        *(q_waveform +i) = (double)(waveform[i])/(double) (0x7fff);
    }
}
