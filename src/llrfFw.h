#ifndef _LLRF_FW_H
#define _LLRF_FW_H

#include <cpsw_api_user.h>
#include <cpsw_api_builder.h>

#define NUM_CH       10
#define NUM_TIMESLOT 18
#define NUM_FB_LOOPS 18

#define MAX_SAMPLES  4096

class IllrfFw;
typedef shared_ptr<IllrfFw> llrfFw;

class IllrfFw : public virtual IEntry {
public:
    static llrfFw create(Path p);

    virtual void setPhaseReferenceOffset(double phase) = 0;
    virtual void setPhaseFeedbackOffset(double phase) = 0;
    virtual void setPhaseGain(double gain) = 0;
    virtual void setAmplGain(double gain) = 0;
    virtual void setReferenceSubtractEnable(bool enable) = 0;
    virtual void setPhaseFeedbackEnable(bool enable) = 0;
    virtual void setAmplFeedbackEnable(bool enable) = 0;
    virtual void setPhaseCorrectionUpperLimit(double limit) = 0;
    virtual void setPhaseCorrectionLowerLimit(double limit) = 0;
    virtual void setAmplCorrectionUpperLimit(double limit) = 0;
    virtual void setAmplCorrectionLowerLimit(double limit) = 0;
    virtual void setAmplDriveUpperLimit(double limit) = 0;
    virtual void setAmplDriveLowerLImit(double limit) = 0;
    virtual void setReferenceChannelWeight(double weight, int channel) = 0;
    virtual void setFeedbackChannelWeight(double weightt, int channel) = 0;
    virtual void setPhaseOffset(double offset, int channel) = 0;
    virtual void setDesiredPhase(double phase, int timeslot) = 0;
    virtual void setDesiredAmpl(double ampl, int timeslot) = 0;

    virtual void getPhaseAllChannels(double *phase) = 0;
    virtual void getAmplAllChannels(double *ampl) = 0;
    virtual void getFeedbackPhaseAllTimeslots(double *phase) = 0;
    virtual void getFeedbackAmplAllTimeslots(double *ampl) = 0;
    virtual void gerReferencePhaseAllTimeslots(double *phase) = 0;
    virtual void getReferenceAmplAllTimeslots(double *ampl) = 0;
    virtual void getPhaseSetAllTimeslots(double *phase) = 0;
    virtual void getAmplSetAllTimeslots(double *ampl) = 0;

    virtual void setAverageWindow(double *window) = 0;
    virtual void getIWaveform(double *i_waveform, int channel) = 0;
    virtual void getQWaveform(double *q_waveform, int channel) = 0;
};


#endif /* _LLRF_FW_H */
