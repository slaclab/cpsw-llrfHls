#ifndef _LLRF_FW_H
#define _LLRF_FW_H

#include <cpsw_api_user.h>
#include <cpsw_api_builder.h>

#define NUM_CH       30
#define NUM_FB_CH    10
#define NUM_DEST     3
#define NUM_WINDOW   3
#define NUM_TIMESLOT 18
#define NUM_FB_LOOPS 18

#define NUM_STATISTICS  (NUM_TIMESLOT + 1)
#define NT_STATISTICS   (NUM_STATISTICS - 1)

#define MAX_SAMPLES  4096


#define NUM_HARMONICS          3
#define HARMONICS_DIX_CS(X)    ((X) * NUM_HARMONICS)
#define HARMONICS_IDX_SN(X)    ((X) * NUM_HARMONICS + 1)
#define ALPHA_DIM              (NUM_HARMONICS*2 + 1)
#define ALPHA_HARMO            (ALPHA_DIM -1)
#define ALPHA_IDX_DC           (ALPHA_DIM -1)



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

    virtual void getPhaseFeedbackStatus(uint32_t *st) = 0;
    virtual void getAmplFeedbackStatus(uint32_t *st) = 0;

    virtual void setStreamEnable(bool enable) = 0;
    virtual void setTimeslotFeedback(bool enable) = 0;
    virtual void setModeConfig(uint32_t mode) = 0;
    virtual void freezeWaveform(bool freeze) = 0;

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
    virtual void setAmplThreshold(double thred) = 0;
    virtual void setAmplDriveUpperLimit(double limit) = 0;
    virtual void setAmplDriveLowerLimit(double limit) = 0;
    virtual void setReferenceChannelWeight(double weight, int channel) = 0;
    virtual void setFeedbackChannelWeight(double weight,  int channel, int dest) = 0;
    virtual void printFeedbackChannelWeight(void) = 0;

    virtual void setAverageWindowPermutationIndex(int idx, int channel) = 0;

    virtual void setPhaseOffset(double offset, int channel) = 0;
    virtual void setDesiredPhase(double phase, int timeslot) = 0;
    virtual void setDesiredAmpl(double ampl, int timeslot) = 0;

    virtual void getPhaseAllChannels(double *phase) = 0;              // get phase for 30 channels
    virtual void getAmplAllChannels(double *ampl) = 0;                // get amplitude for 30 channels
    virtual void getFeedbackPhaseAllTimeslots(double *phase) = 0;
    virtual void getFeedbackAmplAllTimeslots(double *ampl) = 0;
    virtual void getReferencePhaseAllTimeslots(double *phase) = 0;
    virtual void getReferenceAmplAllTimeslots(double *ampl) = 0;
    virtual void getPhaseSetAllTimeslots(double *phase) = 0;
    virtual void getAmplSetAllTimeslots(double *ampl) = 0;

    virtual void setAverageWindow(double *window, int window_idx) = 0;
    virtual void setIWaveformAverageWindow(double *i_waveform, int window_idx) = 0;
    virtual void setQWaveformAverageWindow(double *q_waveform, int window_idx) = 0;
    virtual void getIWaveformAverageWindow(double *i_waveform, int window_idx) = 0;
    virtual void getQWaveformAverageWindow(double *q_waveform, int window_idx) = 0;
    // virtual void getIWaveform(double *i_waveform, int channel) = 0;
    // virtual void getQWaveform(double *q_waveform, int channel) = 0;
    virtual void getIQWaveform(double *i_waveform, double *q_waveform, int channel) = 0;

    virtual void setAmplCoeff(double coeff, int channel) = 0;
    virtual void setAmplNorm(double norm, int dest_idx) = 0;
    virtual void getAmplNorm(double *norm, int dest_idx) = 0;
    virtual void setRecalNorm(bool flag, int dest_idx) = 0;
    virtual void getRecalNorm(uint8_t *flag, int dest_idx) =0;
    virtual void setVarGain(double gain) = 0;
    virtual void setVarNtGain(double gain) = 0;
    virtual void getVarPhaseAllTimeslots(double *var) = 0;
    virtual void getVarAmplAllTimeslots(double *var) = 0;
    virtual void getVarBeamVoltageAllTimeslots(double *var) = 0;
    virtual void getAvgPhaseAllTimeslots(double *avg) = 0;
    virtual void getAvgAmplAllTimeslots(double *avg) = 0;
    virtual void getAvgBeamVoltageAllTimeslots(double *avg) = 0;

    virtual void getVarPhaseAllChannels(double *var) = 0;
    virtual void getVarAmplAllChannels(double *var) = 0;
    virtual void getAvgPhaseAllChannels(double *var) = 0;
    virtual void getAvgAmplAllChannels(double *var) = 0;

    virtual void setOpMode(bool mode) = 0;
    virtual void setPhaseAdaptiveGain(double gain) = 0;
    virtual void setAmplAdaptiveGain(double gain) = 0;
    virtual void setPhaseDistbGain(double gain) = 0;
    virtual void setAmplDistbGain(double gain) = 0;
    virtual void setHarmonicsCs(double *cs, int order) = 0;
    virtual void setHarmonicsSn(double *sn, int order) = 0;

    virtual void getPhaseAlpha(double *alpha) = 0;
    virtual void getAmplAlpha(double *alpha) = 0;
};


#endif /* _LLRF_FW_H */
