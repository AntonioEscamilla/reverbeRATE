/*
  ==============================================================================

    StereoStereoHeadTailPartConv.cpp
    Created: 2 Dec 2016 5:25:23pm
    Author:  Tony Ducks

  ==============================================================================
*/

#include "StereoHeadTailPartConv.h"

//==============================================================================
StereoHeadTailPartConv::StereoHeadTailPartConv(RemainBuffer* _remainBuffer, AccuBuffer* _accBuff):pool(1), remainBuffer(_remainBuffer), accBuff(_accBuff){
    conv_size_block = ACCBUFFSIZE;
    host_size_block = ACCBUFFSIZE;
    size_partition = nextPowerOfTwo(conv_size_block);
    size_fft = (2*size_partition);
    accBuff->definirFFT(size_fft);
    String path("/Users/TonyDucks/Documents/Xcode/Part&ThreadConvClass/Source/IRs/IR1_stereo@44100.wav");
    readIR(path);
    irZeroPadded = new AudioSampleBuffer(2,1);
    remainBuffer->ptr_global = 0;
    accBuff->accIndex = 0;
    resetPartitions();
}

//==============================================================================
StereoHeadTailPartConv::StereoHeadTailPartConv(RemainBuffer* _remainBuffer, AccuBuffer* _accBuff, bool _isDummy):pool(1), remainBuffer(_remainBuffer), accBuff(_accBuff){
    isDummy = _isDummy;
    conv_size_block = ACCBUFFSIZE;
    host_size_block = ACCBUFFSIZE;
    size_partition = nextPowerOfTwo(conv_size_block);
    size_ir = size_partition;
    size_fft = (2*size_partition);
    accBuff->definirFFT(size_fft);
    irAudioBuffer = new AudioSampleBuffer(2,size_ir);   //if dummy we use an impulse as IR of size = size_partition
    irAudioBuffer->clear();
    irAudioBuffer->setSample(0, 0, 1.0f);
    irAudioBuffer->setSample(1, 0, 1.0f);
    irZeroPadded = new AudioSampleBuffer(2,1);
    remainBuffer->ptr_global = 0;
    accBuff->accIndex = 0;
    resetPartitions();
}

//==============================================================================
StereoHeadTailPartConv::StereoHeadTailPartConv(RemainBuffer* _remainBuffer, AccuBuffer* _accBuff, const String& _path):pool(1), remainBuffer(_remainBuffer), accBuff(_accBuff){
    conv_size_block = ACCBUFFSIZE;
    host_size_block = ACCBUFFSIZE;
    size_partition = nextPowerOfTwo(conv_size_block);
    size_fft = (2*size_partition);
    accBuff->definirFFT(size_fft);
    readIR(_path);
    irZeroPadded = new AudioSampleBuffer(2,1);
    remainBuffer->ptr_global = 0;
    accBuff->accIndex = 0;
    resetPartitions();
}

//==============================================================================
StereoHeadTailPartConv::~StereoHeadTailPartConv(){
    //accBuff->clearFFT();
    pool.removeAllJobs (true, 2000);
}

//==============================================================================
void StereoHeadTailPartConv::resetPartitions(){
    total_parts = ceil(size_ir/(1.0f*size_partition));
    head_parts = 1;
    tail_parts = total_parts - head_parts;
    
    irZeroPadded->setSize(2, total_parts*size_partition);
    irZeroPadded->clear();
    irZeroPadded->copyFrom(0, 0, irAudioBuffer->getReadPointer(0), size_ir);
    irZeroPadded->copyFrom(1, 0, irAudioBuffer->getReadPointer(1), size_ir);
        
    //DEFINICION DE OWNEDARRAY CON OVERLAPADDCONVOLVERS
    headConvolversArrayL.clear(true);
    headConvolversArrayR.clear(true);
    for (int i=0; i<head_parts; i++) {
        OverlapAddConvolver* headConvolverL = new OverlapAddConvolver(size_partition,conv_size_block);
        headConvolverL->init_h(irZeroPadded->getReadPointer(0, i*size_partition));
        headConvolverL->init_remainBuffer(remainBuffer->getWritePointer(0), remainBuffer->getSize());
        headConvolversArrayL.add(headConvolverL);
        
        OverlapAddConvolver* headConvolverR = new OverlapAddConvolver(size_partition,conv_size_block);
        headConvolverR->init_h(irZeroPadded->getReadPointer(1, i*size_partition));
        headConvolverR->init_remainBuffer(remainBuffer->getWritePointer(1), remainBuffer->getSize());
        headConvolversArrayR.add(headConvolverR);
    }
    
    tailConvolversArray.clear(true);
    for (int i=head_parts; i<total_parts; i++) {
        OverlapAddConvolver* tailConvolverL = new OverlapAddConvolver(size_partition,conv_size_block);
        tailConvolverL->init_h(irZeroPadded->getReadPointer(0, i*size_partition));
        tailConvolverL->init_remainBuffer(remainBuffer->getWritePointer(0), remainBuffer->getSize());
        tailConvolversArray.add(tailConvolverL);
        
        OverlapAddConvolver* tailConvolverR = new OverlapAddConvolver(size_partition,conv_size_block);
        tailConvolverR->init_h(irZeroPadded->getReadPointer(1, i*size_partition));
        tailConvolverR->init_remainBuffer(remainBuffer->getWritePointer(1), remainBuffer->getSize());
        tailConvolversArray.add(tailConvolverR);
    }
}

//==============================================================================
int StereoHeadTailPartConv::getBffSize(){
    return host_size_block;
}

//==============================================================================
void StereoHeadTailPartConv::setBffSize(int size){
    pool.removeAllJobs (true, 2000);
    if (size < ACCBUFFSIZE){
        host_size_block = size;
        conv_size_block = ACCBUFFSIZE;
        accumulateInputSamplesInBiggerBuffer = true;
    }else{
        host_size_block = size;
        conv_size_block = size;
        accumulateInputSamplesInBiggerBuffer = false;
    }
    size_partition = nextPowerOfTwo(conv_size_block);
    size_fft = (2*size_partition);
    remainBuffer->ptr_global = 0;
    accBuff->accIndex = 0;
    accBuff->definirFFT(size_fft);
    resetPartitions();
}

//==============================================================================
void StereoHeadTailPartConv::readIR(const String& path){
    AudioFormatManager audioFormatManager;
    audioFormatManager.registerBasicFormats();
    File sfile(path);
    AudioFormatReader* audioFormatReader;
    audioFormatReader = audioFormatManager.createReaderFor(sfile);
    hsr = audioFormatReader->sampleRate;
    size_ir = audioFormatReader->lengthInSamples;
    irAudioBuffer = new AudioSampleBuffer(2,size_ir);
    irAudioBuffer->clear();
    audioFormatReader->read(irAudioBuffer, 0, size_ir, 0, true, true);
    deleteAndZero(audioFormatReader);
    audioFormatManager.clearFormats();
}

//==============================================================================
void StereoHeadTailPartConv::loadNewIR(const String& _path){
    pool.removeAllJobs (true, 2000);
    accBuff->accIndex = 0;
    accBuff->definirFFT(size_fft);
    readIR(_path);
    resetPartitions();
}

//==============================================================================
void StereoHeadTailPartConv::prepareForIRLoad(int _size_ir){
    pool.removeAllJobs (true, 2000);
    accBuff->accIndex = 0;
    accBuff->definirFFT(size_fft);
    // equivalente  readIR() solo que se hace la escritura en irAudioBuffer despues con la funcion de VMS
    size_ir = _size_ir;
    irAudioBuffer = new AudioSampleBuffer(2,size_ir);
    irAudioBuffer->clear();
    //.... despues de tener la IR en irAudioBuffer, hay que llamar a resetPartitions().
}

//==============================================================================
void StereoHeadTailPartConv::prepareForNewSynthIR(){
    irAudioBuffer->clear();
    //.... despues de tener la IR en irAudioBuffer, hay que llamar a resetPartitions().
}

//==============================================================================
void StereoHeadTailPartConv::prepareForFutureSynthIR(int _size_ir){
    pool.removeAllJobs (true, 2000);
    //    accBuff->accIndex = 0;
    //    accBuff->definirFFT(size_fft);
    size_ir = _size_ir;
    irAudioBuffer = new AudioSampleBuffer(2,size_ir);
    irAudioBuffer->clear();
}

//==============================================================================
void StereoHeadTailPartConv::removeJobs(){
    pool.removeAllJobs (true, 2000);
    accBuff->accIndex = 0;
    accBuff->definirFFT(size_fft);
}

//==============================================================================
float* StereoHeadTailPartConv::getIrAudioBufferPointer(int chan){
    return irAudioBuffer->getWritePointer(chan);
}

//==============================================================================
void StereoHeadTailPartConv::processBlock(const float** inputData,float** outputData){
    
    const float* readPointerL = inputData[0];
    const float* readPointerR = inputData[0];
    
    float* writePointerL = outputData[0];
    float* writePointerR = outputData[1];
    
    int accIndex = accBuff->accIndex;
    if (accumulateInputSamplesInBiggerBuffer) {
        for (int i=0; i<host_size_block; i++){
            accBuff->xL[i+accIndex][0]=*(readPointerL+i);
            accBuff->xR[i+accIndex][0]=*(readPointerR+i);
        }
        accIndex+=host_size_block;
        if(ACCBUFFSIZE - accIndex < host_size_block){                          //verificar si el accBuffer se desbordaría el siguiente frame
            for (int i=accIndex; i<ACCBUFFSIZE; i++){                          //y en ese caso... se llena con ceros hasta completar ACCBUFFSIZE
                accBuff->xL[i][0]= 0.0f;
                accBuff->xR[i][0]= 0.0f;
            }
            accIndex = ACCBUFFSIZE;
        }
    }else{
        for (int i=0; i<conv_size_block; i++){
            accBuff->xL[i][0]=*(readPointerL+i);
            accBuff->xR[i][0]=*(readPointerR+i);
        }
    }
    
    if (accIndex == ACCBUFFSIZE || !accumulateInputSamplesInBiggerBuffer) {
        fftw_execute(accBuff->TDx2XL);                                            //FFT X
        fftw_execute(accBuff->TDx2XR);
        
        for (int j=0; j<head_parts; j++){
            int writeOut_ptr=remainBuffer->ptr_global + (j*conv_size_block);        //El writeOut_ptr inicia donde esta el global y cada particion...
            headConvolversArrayL[j]->setInputPtr(accBuff->XL);                      //lo desplaza size_block
            headConvolversArrayL[j]->setOutPtr(writeOut_ptr);
            headConvolversArrayL[j]->process();                     //X=fft(inputBuffer) y writeOut_ptr=en que parte del remainBuffer...
                                                                    //se escribe resultado de conv
            headConvolversArrayR[j]->setInputPtr(accBuff->XR);
            headConvolversArrayR[j]->setOutPtr(writeOut_ptr);
            headConvolversArrayR[j]->process();
        }
        
        for (int i=0; i<tail_parts; i++){
            int writeOut_ptr=remainBuffer->ptr_global + ((i+head_parts)*conv_size_block);
            tailConvolversArray[2*i]->setInputPtr(accBuff->XL);
            tailConvolversArray[2*i]->setOutPtr(writeOut_ptr);
            
            tailConvolversArray[2*i+1]->setInputPtr(accBuff->XR);
            tailConvolversArray[2*i+1]->setOutPtr(writeOut_ptr);
        }
        
        TailThreadPoolJob* newTailJob = new TailThreadPoolJob(tailConvolversArray);
        pool.addJob (newTailJob, true);
        
        accIndex = 0;
    }
    
    if (accumulateInputSamplesInBiggerBuffer) {
        for (int i=0; i<host_size_block; i++){
            *(writePointerL+i) = remainBuffer->getSample(0, (remainBuffer->ptr_global+i)%remainBuffer->getSize());
            remainBuffer->setSample(0, (remainBuffer->ptr_global+i)%remainBuffer->getSize(), 0.0f);
            
            *(writePointerR+i) = remainBuffer->getSample(1, (remainBuffer->ptr_global+i)%remainBuffer->getSize());
            remainBuffer->setSample(1, (remainBuffer->ptr_global+i)%remainBuffer->getSize(), 0.0f);
        }
        remainBuffer->ptr_global+= host_size_block;
    }else{
        for (int i=0; i<conv_size_block; i++){
            *(writePointerL+i) = remainBuffer->getSample(0, (remainBuffer->ptr_global+i)%remainBuffer->getSize());    //leer RmBuffer y enviar a la salida
            remainBuffer->setSample(0, (remainBuffer->ptr_global+i)%remainBuffer->getSize(), 0.0f);                   //Se limpia esa parte del buffer
            
            *(writePointerR+i) = remainBuffer->getSample(1, (remainBuffer->ptr_global+i)%remainBuffer->getSize());
            remainBuffer->setSample(1, (remainBuffer->ptr_global+i)%remainBuffer->getSize(), 0.0f);
        }
        remainBuffer->ptr_global+= conv_size_block;
    }
    
    accBuff->accIndex = accIndex;
}