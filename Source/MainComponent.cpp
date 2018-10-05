/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent():readAheadThread("read Ahead thread"){
    
    // Device manager
    audioDeviceManager = new AudioDeviceManager();
    audioDeviceManager->initialise(1, 2, 0, true);
    readAheadThread.startThread(3);
    
    // Cambiar Fs @ 48000
    audioDeviceManager->getAudioDeviceSetup(audioDeviceSetup);
    audioDeviceSetup.sampleRate=48000;
    audioDeviceManager->setAudioDeviceSetup(audioDeviceSetup, true);
    
    dummyIndex = 1;
    remainBuffer = new RemainBuffer(2,480000);
    accBuff = new AccuBuffer();
    StereoHeadTailPartConv* kernel1 = new StereoHeadTailPartConv(remainBuffer, accBuff, getNonDummyIndex());
    StereoHeadTailPartConv* kernel2 = new StereoHeadTailPartConv(remainBuffer, accBuff, getDummyIndex());
    convkernells.add(kernel1);
    convkernells.add(kernel2);
    changeConvKernellFlag = false;
    prepareForFutureSynthFlag = false;
    
    popupButton = new ImageButton ("irPopUp");
    Image im1 = ImageCache::getFromMemory (BinaryData::IRboton_png, BinaryData::IRboton_pngSize);
    Colour bl = Colours::transparentBlack;
    popupButton->setImages (true, true, true, im1, 1.0f, bl, im1, 1.0f, bl, im1, 1.0f, bl);
    popupButton->setTriggeredOnMouseDown (true);
    popupButton->addListener (this);
    addAndMakeVisible (popupButton);
    
    reverbButton = new ImageButton ("wave");
    Image im2 = ImageCache::getFromMemory (BinaryData::waves_gif, BinaryData::waves_gifSize);
    reverbButton->setImages (true, true, true, im2, 1.0f, bl, im2, 1.0f, bl, im2, 1.0f, Colour (0xff42DB82));
    reverbButton->setTriggeredOnMouseDown (true);
    reverbButton->setClickingTogglesState(true);
    reverbButton->addListener (this);
    addAndMakeVisible (reverbButton);
    
    setSize (600, 400);
}

MainContentComponent::~MainContentComponent(){
    audioDeviceManager->removeAudioCallback(this);
}

//==============================================================================
void MainContentComponent::paint (Graphics& g){
    g.fillAll (Colour (0xff001F36));

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    g.drawFittedText("Carga una IR y escucha tu voz en un recinto acustico",getWidth()/2-75,getHeight()/2-25,150,50, Justification::centred,2);
    g.setColour(Colour (0xff42DB82));
    g.fillEllipse(50, getHeight()/2 - 75, 150, 150 );
    
    g.setColour(Colour (0xffFFFFFF));
    g.fillEllipse(getWidth() - 50 - 150, getHeight()/2 - 75, 150, 150);
}

//==============================================================================
void MainContentComponent::resized(){
    popupButton->setBounds(50, getHeight()/2 - 75, 150, 150);
    reverbButton->setBounds(getWidth() - 50 - 150, getHeight()/2 - 75, 150, 150);

}

//==============================================================================
void MainContentComponent::audioDeviceIOCallback(const float** inputData,int InputChannels,float** outputData,int OutputChannels,int numSamples){
    
    if (changeConvKernellFlag){
        setDummyIndex(getNonDummyIndex());
        changeConvKernellFlag = false;
    }
    int currentWorkingKernell = getNonDummyIndex();
    
    if (convkernells[currentWorkingKernell]->getBffSize() != numSamples) {
        convkernells[currentWorkingKernell]->setBffSize(numSamples);
        convkernells[getDummyIndex()]->setBffSize(numSamples);
    }
    
    convkernells[currentWorkingKernell]->processBlock(inputData, outputData);
}

//==============================================================================
void MainContentComponent::audioDeviceAboutToStart (AudioIODevice* device){
    Logger::writeToLog ("actual AudioDevice SampleRate: --> " + String(device->getCurrentSampleRate()));
}

//==============================================================================
void MainContentComponent::buttonClicked(Button* button) {
    if (button == popupButton){
        int result = getDummyPopupMenu().showAt(popupButton);
        
        if (result == 0){
        }
        else if (result == 1){
            File appLocation (File::getSpecialLocation(File::currentExecutableFile));
            String fullPathString = appLocation.getParentDirectory().getParentDirectory().getFullPathName() + "/Resources/IR1.wav";
            convkernells[getDummyIndex()]->loadNewIR(fullPathString);
            changeConvKernellFlag = true;
        }
        else if (result == 2){
            File appLocation (File::getSpecialLocation(File::currentExecutableFile));
            String fullPathString = appLocation.getParentDirectory().getParentDirectory().getFullPathName() + "/Resources/IR2.wav";
            convkernells[getDummyIndex()]->loadNewIR(fullPathString);
            changeConvKernellFlag = true;
        }
        else if (result == 3){
            File appLocation (File::getSpecialLocation(File::currentExecutableFile));
            String fullPathString = appLocation.getParentDirectory().getParentDirectory().getFullPathName() + "/Resources/IR3.wav";
            convkernells[getDummyIndex()]->loadNewIR(fullPathString);
            changeConvKernellFlag = true;
        }
        else if (result == 4){
            File appLocation (File::getSpecialLocation(File::currentExecutableFile));
            String fullPathString = appLocation.getParentDirectory().getParentDirectory().getFullPathName() + "/Resources/IR4.wav";
            convkernells[getDummyIndex()]->loadNewIR(fullPathString);
            changeConvKernellFlag = true;
        }
        else if (result == 5){
            File appLocation (File::getSpecialLocation(File::currentExecutableFile));
            String fullPathString = appLocation.getParentDirectory().getParentDirectory().getFullPathName() + "/Resources/IR5.wav";
            convkernells[getDummyIndex()]->loadNewIR(fullPathString);
            changeConvKernellFlag = true;
        }
    }
    else if (button == reverbButton){
        if (reverbButton->getToggleState()){
            audioDeviceManager->addAudioCallback(this);
        }else {
            audioDeviceManager->removeAudioCallback(this);
            remainBuffer->clear();
            convkernells[getNonDummyIndex()]->removeJobs();
        }
    }
}

//==============================================================================
PopupMenu MainContentComponent::getDummyPopupMenu(){
    PopupMenu m;
    m.addItem (1, "St Andrew's Church", true, false);
    m.addItem (2, "Abernyte Grain Silo", true, false);
    m.addItem (3, "Perth City Hall", true, false);
    m.addItem (4, "German 1950's Plate", true, false);
    m.addItem (5, "Swiss Analog Spring Reverb", true, false);

    return m;
}
