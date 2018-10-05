/*
  ==============================================================================

    StereoHeadTailPartConv.h
    Created: 2 Dec 2016 5:25:23pm
    Author:  Tony Ducks

  ==============================================================================
*/

#ifndef STEREOHEADTAILPARTCONV_H_INCLUDED
#define STEREOHEADTAILPARTCONV_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"
#include "fftw3.h"
#include "OverlapAddConvolver.h"
#include "TailThreadPoolJob.h"

#define ACCBUFFSIZE 4096

class RemainBuffer{
public:
    RemainBuffer(int channels, int lenght){
        remainBuffer = new AudioSampleBuffer(channels,lenght);
        ptr_global = 0;
    }
    
    int getSize(){
        return remainBuffer->getNumSamples();
    }
    
    float* getWritePointer(int _channel){
        return remainBuffer->getWritePointer(_channel);
    }
    
    float getSample (int _channel, int _sampleIndex){
        return remainBuffer->getSample(_channel, _sampleIndex);
    }
    
    void setSample (int _destChannel, int _destSample, float _val){
        remainBuffer->setSample(_destChannel, _destSample, _val);
    }
    
    void clear(){
        //remainBuffer->applyGainRamp(ptr_global, 100000, 1.0f, 0.0f);
        remainBuffer->clear();
        ptr_global = 0;
    }
    
    int                                     ptr_global;
    
private:
    ScopedPointer<AudioSampleBuffer>        remainBuffer;
};

class AccuBuffer{
public:
    AccuBuffer(){
        definirFFT(1);
        accIndex = 0;
    }
    
    void definirFFT(int _size_fft){
        xL = ( fftw_complex*) fftw_malloc( sizeof(fftw_complex) * _size_fft);
        XL = ( fftw_complex*) fftw_malloc( sizeof(fftw_complex) * _size_fft);
        memset (xL, 0, sizeof( fftw_complex ) * _size_fft);
        TDx2XL = fftw_plan_dft_1d(_size_fft, xL, XL, FFTW_FORWARD, FFTW_ESTIMATE);
        
        xR = ( fftw_complex*) fftw_malloc( sizeof(fftw_complex) * _size_fft);
        XR = ( fftw_complex*) fftw_malloc( sizeof(fftw_complex) * _size_fft);
        memset (xR, 0, sizeof( fftw_complex ) * _size_fft);
        TDx2XR = fftw_plan_dft_1d(_size_fft, xR, XR, FFTW_FORWARD, FFTW_ESTIMATE);
    }
    void clearFFT(){
        fftw_destroy_plan(TDx2XL);
        fftw_free(xL);
        fftw_free(XL);
        fftw_destroy_plan(TDx2XR);
        fftw_free(xR);
        fftw_free(XR);
    }
    
    int                                 accIndex;
    fftw_complex                        *xL, *XL, *xR, *XR;
    fftw_plan                           TDx2XL, TDx2XR;
};

class StereoHeadTailPartConv
{
public:
    
    StereoHeadTailPartConv(RemainBuffer* _remainBuffer, AccuBuffer* _accBuff);
    StereoHeadTailPartConv(RemainBuffer* _remainBuffer, AccuBuffer* _accBuff, bool _isDummy);
    StereoHeadTailPartConv(RemainBuffer* _remainBuffer, AccuBuffer* _accBuff, const String& _path);
    virtual ~StereoHeadTailPartConv();
//    void definirFFT(int );
//    void clearFFT();
    void init_h(const float* _h);
    int getBffSize();
    void setBffSize(int size);
    void readIR(const String& path);
    void loadNewIR(const String& path);
    void prepareForIRLoad(int _size_ir);
    void prepareForNewSynthIR();
    void prepareForFutureSynthIR(int _size_ir);
    void resetPartitions();
    void removeJobs();
    void processBlock(const float** inputData,float** outputData);
    float* getIrAudioBufferPointer(int _channel);
    int getSizefft();

private:
    int                                 total_parts, tail_parts, head_parts;
    OwnedArray<OverlapAddConvolver>     headConvolversArrayL, headConvolversArrayR;
    OwnedArray<OverlapAddConvolver>     tailConvolversArray;
    ThreadPool                          pool;
    RemainBuffer*                       remainBuffer;
    AccuBuffer*                         accBuff;
    
    int                                 hsr;
    int                                 size_ir, conv_size_block, size_fft, size_partition;
    int                                 host_size_block;
    
//    fftw_complex                        *xL, *XL, *xR, *XR;
//    fftw_plan                           TDx2XL, TDx2XR;
    
    ScopedPointer<AudioSampleBuffer>    irAudioBuffer, irZeroPadded;
    
    bool                                accumulateInputSamplesInBiggerBuffer = false;
    bool                                isDummy;
};




#endif  // STEREOHEADTAILPARTCONV_H_INCLUDED
