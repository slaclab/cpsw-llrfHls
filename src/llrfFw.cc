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


extern "C" {
    int32_t     ComplexAvgWindow = 0;
}


class CllrfFwAdapt;
typedef shared_ptr<CllrfFwAdapt> llrfFwAdapt;

class CllrfFwAdapt : public IllrfFw, public IEntryAdapt {
private:
    bool   valid_cAvgWnd;
    double ref_weight_input[NUM_FB_CH], ref_weight_norm[NUM_FB_CH];
    double fb_weight_input[NUM_FB_CH],  fb_weight_norm[NUM_FB_CH];

    struct {
        double i[MAX_SAMPLES];
        double q[MAX_SAMPLES];
    } wnd[NUM_WINDOW];

    double norm(double i[], double q[]);
    void   updateComplexWindow(int window_idx);

protected:
    Path pLlrfFeedbackWrapper_;
    Path pLlrfHls_;
//    Path pFeedbackWindow_;
    Path pIQWaveform_;

    /* system configuration */
    ScalVal_RO version_;                  // llrfHls firmware version
    ScalVal_RO num_timeslot_;             // number of timeslot
    ScalVal_RO num_channel_;              // number of channels
    ScalVal_RO num_window_;               // number of average window
    ScalVal_RO max_pulseLength_;          // max pulse length
    /* diagnostics */
    ScalVal_RO counter_;                  // llrfHls loop counter
    ScalVal_RO drop_counter_;             // stream drop counter
    /* stream control */
    ScalVal    stream_enable_;            // sream enable:1, disable: 0
    ScalVal    disable_timeslot_;         // disable timeslot feedback
    ScalVal    mode_config_;              // trigger mode, disable:0, accel: 1, stdby: 2, accel_or_stdby: 3
    ScalVal    freeze_wf_;                // freeze iq waveform reading

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
    DoubleVal p_offset_[NUM_FB_CH];                     // phase offset for each channel, array[10]
    DoubleVal p_des_[NUM_TIMESLOT];                     // desired phase, PDES, for each timeslot, array[18]
    DoubleVal a_des_[NUM_TIMESLOT];                     // desired amplitude, ADES, for each timeslot, array[18]

    DoubleVal_RO p_ch_;                      // window averaged phase for each channel, array[30]
    DoubleVal_RO a_ch_;                      // window averaged amplitude for each channel, array[30]
    DoubleVal_RO p_fb_;                      // channel averaged phase for each timeslot, array[18]
    DoubleVal_RO a_fb_;                      // channel averaged amplitude for each timeslot, array[18]
    DoubleVal_RO p_ref_;                     // channel averaged phase for reference, for each timeslot, array[18]
    DoubleVal_RO a_ref_;                     // channel averaged amplitude for reference, for each timeslot, array[18]

    DoubleVal_RO p_set_;                  // phase set value for each timeslot, array[18]
    DoubleVal_RO a_set_;                  // ampliktude set value for each timeslot, array[18]


    // amplitude conversion, configuration
    DoubleVal    a_conv_coeff_[NUM_FB_CH];   // amplitude conversion coefficient per channel array[10];
    DoubleVal    a_norm_;                    // normalization factor for aset value
    // jitter calculation (variance), configuration
    DoubleVal    var_gain_;                  // gain for variance and mean calculation (single pole algorithm)
    // jitter calculation readout
    DoubleVal_RO var_p_fb_;                  // variance for phase, timeslot aware, array[18]
    DoubleVal_RO var_a_fb_;                  // variance for amplitude, timeslot aware, array[18]
    DoubleVal_RO var_bv_;                    // variance for beam voltage, timeslot aware, array[18]
    DoubleVal_RO mean_p_fb_;                 // average for phase, timeslot aware, array[18]
    DoubleVal_RO mean_a_fb_;                 // average for amplitude, timeslot aware, array[18]
    DoubleVal_RO mean_bv_;                   // average for beam voltage, timeslot aware, array[18]

    ScalVal      avg_window_[NUM_WINDOW];                // average window
    ScalVal      complex_window_[NUM_WINDOW];           // complex window, combinded i and q

    ScalVal_RO   iq_waveform_ch_[NUM_FB_CH];     // combined i and q waveform for each channel


    // for adaptive feedback
    // configuration
    ScalVal      op_mode_;                   // operation, backward comptible vs. adaptive feedback
    DoubleVal    p_adaptive_gain_;           // phase feedback gain for adaptive mode
    DoubleVal    a_adaptive_gain_;           // amplitude feedback gain for adaptive mode
    DoubleVal    p_distb_gain_;              // disturbance gain for phase
    DoubleVal    a_distb_gain_;              // distrubance gain for amplitude
    DoubleVal    harmo_cs1_;                 // cos vector for 1st harmonics
    DoubleVal    harmo_sn1_;                 // sin vector for 1st harmonics
    DoubleVal    harmo_cs2_;                 // cos vector for 2nd harmonics
    DoubleVal    harmo_sn2_;                 // sin vector for 2nd harmnics
    DoubleVal    harmo_cs3_;                 // cos vector for 3rd harmonics
    DoubleVal    harmo_sn3_;                 // sin vector for 3rd harmonics
    // status
    DoubleVal_RO    p_alpha_;                   // alpha vector phase
    DoubleVal_RO    a_alpha_;                   // alpha vector for amplitude


public:
    CllrfFwAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie);

    virtual void getVersion(uint32_t *version);
    virtual void getNumTimeslot(uint32_t *num);
    virtual void getNumChannel(uint32_t *num);
    virtual void getNumWindow(uint32_t *num);
    virtual void getMaxPulseLength(uint32_t *len);
    virtual void getCounter(uint32_t *counter);
    virtual void getDropCounter(uint32_t *dropCounter);

    virtual void setStreamEnable(bool enable);
    virtual void setTimeslotFeedback(bool enable);
    virtual void setModeConfig(uint32_t mode);
    virtual void freezeWaveform(bool freeze);


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

    virtual void setAverageWindow(double *window, int window_idx);
    virtual void setIWaveformAverageWindow(double *i_waveform, int window_idx);
    virtual void setQWaveformAverageWindow(double *q_waveform, int window_idx);
    virtual void getIQWaveform(double *i_waveform, double *q_waveform, int channel);

    virtual void setAmplCoeff(double coeff, int channel);
    virtual void setAmplNorm(double norm);
    virtual void setVarGain(double gain);
    virtual void getVarPhaseAllTimeslots(double *var);
    virtual void getVarAmplAllTimeslots(double *var);
    virtual void getVarBeamVoltageAllTimeslots(double *var);
    virtual void getAvgPhaseAllTimeslots(double *avg);
    virtual void getAvgAmplAllTimeslots(double *avg);
    virtual void getAvgBeamVoltageAllTimeslots(double *avg);    

    virtual void setOpMode(bool mode);
    virtual void setPhaseAdaptiveGain(double gain);
    virtual void setAmplAdaptiveGain(double gain);
    virtual void setPhaseDistbGain(double gain);
    virtual void setAmplDistbGain(double gain);
    virtual void setHarmonicsCs(double *cs, int order);
    virtual void setHarmonicsSn(double *sn, int order);

    virtual void getPhaseAlpha(double *alpha);
    virtual void getAmplAlpha(double *alpha);
};


llrfFw IllrfFw::create(Path p)
{
    return IEntryAdapt::check_interface<llrfFwAdapt, DevImpl>(p);
}


CllrfFwAdapt::CllrfFwAdapt(Key &k, ConstPath p, shared_ptr<const CEntryImpl> ie):
    IEntryAdapt(k, p, ie),
    pLlrfFeedbackWrapper_(p->findByName("")),
    pLlrfHls_(       pLlrfFeedbackWrapper_->findByName("LlrfHls")),
    pIQWaveform_(    pLlrfFeedbackWrapper_->findByName("IQWaveform")),

    version_(         IScalVal_RO::create(pLlrfHls_->findByName("VERSION_V"))),
    num_timeslot_(    IScalVal_RO::create(pLlrfHls_->findByName("NUM_TIMESLOT_V"))),
    num_channel_(     IScalVal_RO::create(pLlrfHls_->findByName("NUM_CHANNELS_V"))),
    num_window_(      IScalVal_RO::create(pLlrfHls_->findByName("NUM_WINDOW_V"))),
    max_pulseLength_( IScalVal_RO::create(pLlrfHls_->findByName("MAX_PULSE_LEN_V"))),
    counter_(         IScalVal_RO::create(pLlrfHls_->findByName("COUNTER_V"))),
    drop_counter_(    IScalVal_RO::create(pLlrfHls_->findByName("DROP_COUNTER_V"))),

    stream_enable_(   IScalVal::create(pLlrfHls_->findByName("STREAM_ENABLE"))),
    disable_timeslot_(IScalVal::create(pLlrfHls_->findByName("DISABLE_TIMESLOT_FEEDBACK"))),
    mode_config_(     IScalVal::create(pLlrfFeedbackWrapper_->findByName("MODE_CONFIG"))),
    freeze_wf_(       IScalVal::create(pLlrfFeedbackWrapper_->findByName("FREEZE_SW_WF"))),

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
    ref_weight_(            IDoubleVal::create(pLlrfHls_->findByName("REF_WEIGHT_IN"))),  
    fb_weight_ (            IDoubleVal::create(pLlrfHls_->findByName("FB_WEIGHT_IN"))),
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

    // a_conv_coeff_    // make multiple instaces in function body, 10 instances
    a_norm_(                IDoubleVal::create(pLlrfHls_->findByName("A_NORM"))),           // array[18], get all of timeslot data at once
    var_gain_(              IDoubleVal::create(pLlrfHls_->findByName("VAR_GAIN"))),         // array[18], get all of timeslot data at once
    var_p_fb_(              IDoubleVal_RO::create(pLlrfHls_->findByName("VAR_P_FB"))),      // array[18], get all of timeslot data at once
    var_a_fb_(              IDoubleVal_RO::create(pLlrfHls_->findByName("VAR_A_FB"))),      // array[18], get all of timeslot data at once
    var_bv_(                IDoubleVal_RO::create(pLlrfHls_->findByName("VAR_BV"))),        // array[18], get all of timeslot data at once
    mean_p_fb_(             IDoubleVal_RO::create(pLlrfHls_->findByName("MEAN_P_FB"))),     // array[18], get all of timeslot data at once
    mean_a_fb_(             IDoubleVal_RO::create(pLlrfHls_->findByName("MEAN_A_FB"))),     // array[18], get all of timeslot data at once
    mean_bv_(               IDoubleVal_RO::create(pLlrfHls_->findByName("MEAN_BV"))),       // array[18], get all of timeslot data at once

    // adaptive mode, configuration
    op_mode_(               IScalVal::create(pLlrfHls_->findByName("OP_MODE"))),
    p_adaptive_gain_(       IDoubleVal::create(pLlrfHls_->findByName("P_ADAPTIVE_GAIN"))),
    a_adaptive_gain_(       IDoubleVal::create(pLlrfHls_->findByName("A_ADAPTIVE_GAIN"))),
    p_distb_gain_(          IDoubleVal::create(pLlrfHls_->findByName("P_DISTB_GAIN"))),
    a_distb_gain_(          IDoubleVal::create(pLlrfHls_->findByName("A_DISTB_GAIN"))),
    harmo_cs1_(             IDoubleVal::create(pLlrfHls_->findByName("HARMO_CS1"))),      // array[18]
    harmo_sn1_(             IDoubleVal::create(pLlrfHls_->findByName("HARMO_SN1"))),      // array[18]
    harmo_cs2_(             IDoubleVal::create(pLlrfHls_->findByName("HARMO_CS2"))),      // araay[18]
    harmo_sn2_(             IDoubleVal::create(pLlrfHls_->findByName("HARMO_SN2"))),      // array[18]
    harmo_cs3_(             IDoubleVal::create(pLlrfHls_->findByName("HARMO_CS3"))),      // array[18]
    harmo_sn3_(             IDoubleVal::create(pLlrfHls_->findByName("HARMO_SN3"))),      // array[18]
    // adaptive mode, status
    p_alpha_(               IDoubleVal_RO::create(pLlrfHls_->findByName("P_ALPHA"))),      // array[7]
    a_alpha_(               IDoubleVal_RO::create(pLlrfHls_->findByName("A_ALPHA")))       // array[7]


{
    char str_name[80];

    for(int i = 0; i < NUM_FB_CH; i++) {
        sprintf(str_name, "P_OFFSET[%d]",   i);           p_offset_[i]      = IDoubleVal::create(pLlrfHls_->findByName(str_name));
        sprintf(str_name, "IQWaveform[%d]/waveformIQ", i); iq_waveform_ch_[i] = IScalVal_RO::create(pLlrfFeedbackWrapper_->findByName(str_name));
        sprintf(str_name, "A_CONV_COEFF[%d]", i);          a_conv_coeff_[i] = IDoubleVal::create(pLlrfHls_->findByName(str_name));


        ref_weight_input[i] = ref_weight_norm[i] = 0.;    // intitialize the referecne channel weight
        fb_weight_input[i]  = fb_weight_norm[i]  = 0.;    // iniitialize the feeedback channel weight
    }

    for(int i = 0; i < NUM_TIMESLOT; i++) {
        sprintf(str_name, "P_DES[%d]", i); p_des_[i] = IDoubleVal::create(pLlrfHls_->findByName(str_name));
        sprintf(str_name, "A_DES[%d]", i); a_des_[i] = IDoubleVal::create(pLlrfHls_->findByName(str_name));
    }    

    for(int i = 0; i < NUM_WINDOW; i++) {
        if(ComplexAvgWindow) {    /* complex average window */
            valid_cAvgWnd = true;   /* the complex average window is valid */
            sprintf(str_name, "FeedbackWindow[%d]/waveformIQ", i); complex_window_[i] = IScalVal::create(pLlrfFeedbackWrapper_->findByName(str_name));
        } else {   /* non complex average window */
            valid_cAvgWnd = false; /* the complex average window is invalid */
            sprintf(str_name, "FeedbackWindow[%d]/Window", i); avg_window_[i] = IScalVal::create(pLlrfFeedbackWrapper_->findByName(str_name));
       }
    }

    for(int w = 0; w < NUM_WINDOW; w++) {
        for(int i = 0; i < MAX_SAMPLES; i++) {
            wnd[w].i[i] = 0.;
            wnd[w].q[i] = 0.;
        }
    }

}

void CllrfFwAdapt::getVersion(uint32_t *version)
{
    CPSW_TRY_CATCH(version_->getVal(version));
}

void CllrfFwAdapt::getNumTimeslot(uint32_t *num)
{
    CPSW_TRY_CATCH(num_timeslot_->getVal(num));
}

void CllrfFwAdapt::getNumChannel(uint32_t *num)
{
    CPSW_TRY_CATCH(num_channel_->getVal(num));
}

void CllrfFwAdapt::getNumWindow(uint32_t *num)
{
    CPSW_TRY_CATCH(num_window_->getVal(num));
}

void CllrfFwAdapt::getMaxPulseLength(uint32_t *len)
{
    CPSW_TRY_CATCH(max_pulseLength_->getVal(len));
}

void CllrfFwAdapt::getCounter(uint32_t *counter)
{
    CPSW_TRY_CATCH(counter_->getVal(counter));
}

void CllrfFwAdapt::getDropCounter(uint32_t *dropCounter)
{
    CPSW_TRY_CATCH(drop_counter_->getVal(dropCounter));
}

void CllrfFwAdapt::setStreamEnable(bool enable)
{
    CPSW_TRY_CATCH(stream_enable_->setVal(enable?1:0));
}

void CllrfFwAdapt::setTimeslotFeedback(bool enable)
{
    CPSW_TRY_CATCH(disable_timeslot_->setVal(enable?0:1));
}

void CllrfFwAdapt::setModeConfig(uint32_t mode)
{
    CPSW_TRY_CATCH(mode_config_->setVal(mode));
}

void CllrfFwAdapt::freezeWaveform(bool freeze)
{
    CPSW_TRY_CATCH(freeze_wf_->setVal(freeze?1: 0));
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

    ref_weight_input[channel] = weight;

    for(int i = 0; i < NUM_FB_CH; i++) {
        sum += ref_weight_input[i];
    }
    if(sum <0.) return;

    for(int i = 0; i < NUM_FB_CH; i++) {
        ref_weight_norm[i] = ref_weight_input[i] / sum;
    }

    CPSW_TRY_CATCH(ref_weight_->setVal(ref_weight_norm, NUM_FB_CH));
}

void CllrfFwAdapt::setFeedbackChannelWeight(double weight, int channel)
{
    double sum = 0.;

    fb_weight_input[channel] = weight;

    for(int i = 0; i < NUM_FB_CH; i++) {
        sum += fb_weight_input[i];
    }
    if(sum < 0.) return;

    for(int i = 0; i < NUM_FB_CH; i++) {
        fb_weight_norm[i] = fb_weight_input[i] / sum;
    }

    CPSW_TRY_CATCH(fb_weight_->setVal(fb_weight_norm, NUM_FB_CH));
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

void CllrfFwAdapt::getPhaseAllChannels(double *phase)    // get phase for 30 channels
{
    CPSW_TRY_CATCH(p_ch_->getVal(phase, NUM_CH));

    for(int i = 0; i < NUM_CH; i++) {
        *(phase + i) *= (180./M_PI);    // radian to degree
    }
}

void CllrfFwAdapt::getAmplAllChannels(double *ampl)     // get amplitude for 30 channels
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


void CllrfFwAdapt::setAverageWindow(double *window, int window_idx)
{
    if(valid_cAvgWnd) return ;

    double sum = 0.;

    for(int i = 0; i < MAX_SAMPLES; i++) sum += fabs(*(window + i));
    if (sum < 1.) sum = 1.;

    int16_t window_out[MAX_SAMPLES];
 
    for(int i = 0; i < MAX_SAMPLES; i++) {
        window_out[i] = (int16_t)((*(window + i) / sum) * 0x7fff);
    }

    CPSW_TRY_CATCH(avg_window_[window_idx]->setVal((uint16_t*) window_out, MAX_SAMPLES));
}


double CllrfFwAdapt::norm(double i[], double q[])
{
    double sum = 0.;

    for(int k =0; k < MAX_SAMPLES; k++) sum += sqrt(i[k] * i[k] + q[k] * q[k]);

    return sum;
}

void CllrfFwAdapt::updateComplexWindow(int window_idx)
{

    double n = norm(wnd[window_idx].i, wnd[window_idx].q);
    if(n < 1.) n = 1.;

    union {
        struct {
            int16_t i;
            int16_t q;
        }        d16d16[MAX_SAMPLES];
        uint32_t d32[MAX_SAMPLES];
    } wf;    

    for(int i = 0; i < MAX_SAMPLES; i++) {
        wf.d16d16[i].i = (int16_t)((wnd[window_idx].i[i] / n) * 0x7fff);
        wf.d16d16[i].q = (int16_t)((wnd[window_idx].q[i] / n) * 0x7fff);
    }

    CPSW_TRY_CATCH(complex_window_[window_idx]->setVal(wf.d32, MAX_SAMPLES));

}

void CllrfFwAdapt::setIWaveformAverageWindow(double *i_waveform, int window_idx)
{
    if(!valid_cAvgWnd) return;

    for(int i = 0; i < MAX_SAMPLES; i++) wnd[window_idx].i[i] = *(i_waveform + i);

    updateComplexWindow(window_idx);
}

void CllrfFwAdapt::setQWaveformAverageWindow(double *q_waveform, int window_idx)
{
    if(!valid_cAvgWnd) return;

    for(int i = 0; i < MAX_SAMPLES; i++) wnd[window_idx].q[i] = *(q_waveform + i);

    updateComplexWindow(window_idx);

}



void CllrfFwAdapt::getIQWaveform(double *i_waveform, double *q_waveform, int channel)
{
    union {
        struct {
            int16_t i;
            int16_t q;
        }        d16d16[MAX_SAMPLES];
        uint32_t d32[MAX_SAMPLES];
    } wf;

    CPSW_TRY_CATCH(iq_waveform_ch_[channel]->getVal(wf.d32, MAX_SAMPLES));

    for(int i = 0; i < MAX_SAMPLES; i++) {
        *(i_waveform + i) = (double)(wf.d16d16[i].i) / (double)(0x7fff);
        *(q_waveform + i) = (double)(wf.d16d16[i].q) / (double)(0x7fff);
    }
}




void CllrfFwAdapt::setAmplCoeff(double coeff, int channel)
{
    CPSW_TRY_CATCH(a_conv_coeff_[channel]->setVal(coeff));
}

void CllrfFwAdapt::setAmplNorm(double norm)
{
    CPSW_TRY_CATCH(a_norm_->setVal(norm));
}

void CllrfFwAdapt::setVarGain(double gain)
{
    CPSW_TRY_CATCH(var_gain_->setVal(gain));
}

void CllrfFwAdapt::getVarPhaseAllTimeslots(double *var)
{
    CPSW_TRY_CATCH(var_p_fb_->getVal(var, NUM_TIMESLOT));
}

void CllrfFwAdapt::getVarAmplAllTimeslots(double *var)
{
    CPSW_TRY_CATCH(var_a_fb_->getVal(var, NUM_TIMESLOT));
}

void CllrfFwAdapt::getVarBeamVoltageAllTimeslots(double *var)
{
    CPSW_TRY_CATCH(var_bv_->getVal(var, NUM_TIMESLOT));
}

void CllrfFwAdapt::getAvgPhaseAllTimeslots(double *avg)
{
    CPSW_TRY_CATCH(mean_p_fb_->getVal(avg, NUM_TIMESLOT));
}

void CllrfFwAdapt::getAvgAmplAllTimeslots(double *avg)
{
    CPSW_TRY_CATCH(mean_a_fb_->getVal(avg, NUM_TIMESLOT));
}

void CllrfFwAdapt::getAvgBeamVoltageAllTimeslots(double *avg)
{
    CPSW_TRY_CATCH(mean_bv_->getVal(avg, NUM_TIMESLOT));
}


void CllrfFwAdapt::setOpMode(bool mode)
{
    CPSW_TRY_CATCH(op_mode_->setVal(mode?1:0));
}

void CllrfFwAdapt::setPhaseAdaptiveGain(double gain)
{
    CPSW_TRY_CATCH(p_adaptive_gain_->setVal(gain));
}

void CllrfFwAdapt::setAmplAdaptiveGain(double gain)
{
    CPSW_TRY_CATCH(a_adaptive_gain_->setVal(gain));
}

void CllrfFwAdapt::setPhaseDistbGain(double gain)
{
    CPSW_TRY_CATCH(p_distb_gain_->setVal(gain));
}

void CllrfFwAdapt::setAmplDistbGain(double gain)
{
    CPSW_TRY_CATCH(a_distb_gain_->setVal(gain));
}

void CllrfFwAdapt::setHarmonicsCs(double *cs, int order)
{
    switch(order) {
        case 1:
            CPSW_TRY_CATCH(harmo_cs1_->setVal(cs, NUM_TIMESLOT));
            break;
        case 2:
            CPSW_TRY_CATCH(harmo_cs2_->setVal(cs, NUM_TIMESLOT));
            break;
        case 3:
            CPSW_TRY_CATCH(harmo_cs3_->setVal(cs, NUM_TIMESLOT));
            break;
    }
}

void CllrfFwAdapt::setHarmonicsSn(double *sn, int order)
{
    switch(order) {
        case 1:
            CPSW_TRY_CATCH(harmo_sn1_->setVal(sn, NUM_TIMESLOT));
            break;
        case 2:
            CPSW_TRY_CATCH(harmo_sn2_->setVal(sn, NUM_TIMESLOT));
            break;
        case 3:
            CPSW_TRY_CATCH(harmo_sn3_->setVal(sn, NUM_TIMESLOT));
            break;
    }
}

void CllrfFwAdapt::getPhaseAlpha(double *alpha)
{
    CPSW_TRY_CATCH(p_alpha_->getVal(alpha, ALPHA_DIM));
}

void CllrfFwAdapt::getAmplAlpha(double *alpha)
{
    CPSW_TRY_CATCH(a_alpha_->getVal(alpha, ALPHA_DIM));
}
