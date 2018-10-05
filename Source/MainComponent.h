/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "convClass/StereoHeadTailPartConv.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   :  public Component,
                                public AudioIODeviceCallback,
                                public Button::Listener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked (Button* button) override;
    PopupMenu getDummyPopupMenu();
    //void menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/) override;
    
    void audioDeviceIOCallback(const float** inputData,int InputChannels,float** outputData,int OutputChannels,int numSamples) override;
    void audioDeviceAboutToStart (AudioIODevice* device) override;
    void audioDeviceStopped()override{};
    
    //==============================================================================
    void setDummyIndex(int index){ dummyIndex = index;};
    int getDummyIndex(){ return dummyIndex;};
    int getNonDummyIndex(){ return 1 - dummyIndex;};

private:
    ScopedPointer<ImageButton>      popupButton;
    ScopedPointer<ImageButton>      reverbButton;
    
    ScopedPointer<AudioDeviceManager>       audioDeviceManager;
    TimeSliceThread                         readAheadThread;
    AudioDeviceManager::AudioDeviceSetup    audioDeviceSetup;
    
    OwnedArray<StereoHeadTailPartConv>      convkernells;
    ScopedPointer<RemainBuffer>             remainBuffer;
    ScopedPointer<AccuBuffer>               accBuff;
    int                                     dummyIndex;
    bool                                    changeConvKernellFlag;
    bool                                    prepareForFutureSynthFlag;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
