#ifndef _LLRF_FW_H
#define _LLRF_FW_H

#include <cpsw_api_user.h>
#include <cpsw_api_builder.h>

#define NUM_CH       30
#define NUM_WINDOW   3
#define NUM_TIMESLOT 18
#define NUM_FB_LOOPS 18

#define MAX_SAMPLES  4096

class IllrfFw;
typedef shared_ptr<IllrfFw> llrfFw;

class IllrfFw : public virtual IEntry {
public:
    static llrfFw create(Path p);

    virtual void getVersion(uint32_t *version) = 0;
    virtual void getNumTimeslot(uint32_t *num) = 0;
    virtual void getNumChannel(uint32_t *num) = 0;
    virtual void getNumWindow(uint32_t *num) = 0;
    virtual void getMaxPulseLength(uint32_t *len) = 0;
    virtual void getCounter(uint32_t *counter) = 0;
    virtual void getDropCounter(uint32_t *dropCounter) = 0;

    virtual void setPhaseReferenceOffset(double phase) = 0;
    virtual void setPhaseFeedbackOffset(double phase) = 0;
    virtual void setPhaseGain(double gain) = 0;
    virtual void setAmplGain(double gain) = 0;
    virtual void setReferenceSubtractionEnable(bool enable) = 0;
    virtual void setPhaseFeedbackEnable(bool enable) = 0;
    virtual void setAmplFeedbackEnable(bool enable) = 0;
    virtual void setPhaseCorrectionUpperLimit(double limit) = 0;
    virtual void setPhaseCorrectionLowerLimit(double limit) = 0;
    virtual void setAmplCorrectionUpperLimit(double limit) = 0;
    virtual void setAmplCorrectionLowerLimit(double limit) = 0;
    virtual void setAmplDriveUpperLimit(double limit) = 0;
    virtual void setAmplDriveLowerLimit(double limit) = 0;
    virtual void setReferenceChannelWeight(double weight, int channel) = 0;
    virtual void setFeedbackChannelWeight(double weightt, int channel) = 0;
    virtual void setPhaseOffset(double offset, int channel) = 0;
    virtual void setDesiredPhase(double phase, int timeslot) = 0;
    virtual void setDesiredAmpl(double ampl, int timeslot) = 0;

    virtual void getPhaseAllChannels(double *phase) = 0;
    virtual void getAmplAllChannels(double *ampl) = 0;
    virtual void getFeedbackPhaseAllTimeslots(double *phase) = 0;
    virtual void getFeedbackAmplAllTimeslots(double *ampl) = 0;
    virtual void getReferencePhaseAllTimeslots(double *phase) = 0;
    virtual void getReferenceAmplAllTimeslots(double *ampl) = 0;
    virtual void getPhaseSetAllTimeslots(double *phase) = 0;
    virtual void getAmplSetAllTimeslots(double *ampl) = 0;

    virtual void setAverageWindow(double *window, int window_idx) = 0;
    virtual void getIWaveform(double *i_waveform, int channel) = 0;
    virtual void getQWaveform(double *q_waveform, int channel) = 0;
};


#endif /* _LLRF_FW_H */
