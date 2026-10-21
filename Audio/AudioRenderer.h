//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "AudioUtilities.h"
#include "DSP/TestToneGenerator.h"
#include "AudioTypes.h"
#include "DSP/AnimatedFilter.h"
#include "DSP/AnimatedFFT.h"
#include <CoreAudio/CoreAudio.h>

static bool audioOn     = false;
static float gainAmount = 0.75f;
AnimatedFilter filter;

static double getStreamSampleRate(AudioUnit unit, AudioUnitScope scope)
{
    AudioStreamBasicDescription asbd = {};
    UInt32 size = sizeof(asbd);
    checkError(AudioUnitGetProperty(unit,
                                    kAudioUnitProperty_StreamFormat,
                                    scope,
                                    0,
                                    &asbd,
                                    &size),
               "Couldn't get stream format");
    return asbd.mSampleRate;
}


//============================================
//Render callback for sine wave
//============================================
OSStatus processBlock(void *inRefCon,
                      AudioUnitRenderActionFlags *ioActionFlags,
                      const AudioTimeStamp *inTimeStamp,
                      UInt32 inBusNumber,
                      UInt32 inNumberFrames,
                      AudioBufferList *ioData)
{
    auto *renderer = (MyRenderer*) inRefCon;
    //prepare oscillator(renderer, 400.0f)

    renderVoice(renderer, ioData, inNumberFrames);


    //etc

        return noErr;
}

//============================================
//Function for creating an output audio unit
//============================================
void CreateAndConnectOutputUnit(MyRenderer *renderer)
{
    //============================================
    //First, define the output component
    //============================================
    AudioComponentDescription outputCd = {};
    outputCd.componentType             = kAudioUnitType_Output;
    outputCd.componentSubType          = kAudioUnitSubType_DefaultOutput;
    outputCd.componentManufacturer     = kAudioUnitManufacturer_Apple;
    //============================================
    //Second, find the audio unit output component
    //============================================
    AudioComponent comp = AudioComponentFindNext(NULL, &outputCd);
    if (comp == nullptr) {
        printf("can't get output unit");
        exit (-1);
    }

    checkError(AudioComponentInstanceNew(comp, &renderer->outputUnit),
               "Could not open component for output unit");
    //============================================
    //Third, register the render callback
    //============================================
    AURenderCallbackStruct input;
    input.inputProc       = processBlock;
    input.inputProcRefCon = renderer;
    checkError(AudioUnitSetProperty(renderer->outputUnit,
                                    kAudioUnitProperty_SetRenderCallback,
                                    kAudioUnitScope_Input,
                                    0,
                                    &input,
                                    sizeof(input)),
               "AudioUnitSetProperty failed");
    checkError (AudioUnitInitialize(renderer->outputUnit),
                "Could not initialize output unit");

    double hwRate = getStreamSampleRate(renderer->outputUnit, kAudioUnitScope_Output);

    AudioStreamBasicDescription fmt = {};
    UInt32 size = sizeof(fmt);
    checkError(AudioUnitGetProperty(renderer->outputUnit,
                                    kAudioUnitProperty_StreamFormat,
                                    kAudioUnitScope_Input, 0, &fmt, &size),
               "Couldn't get input format");

    fmt.mSampleRate = hwRate;   // keep channels / Float32 / non-interleaved as-is
    checkError(AudioUnitSetProperty(renderer->outputUnit,
                                    kAudioUnitProperty_StreamFormat,
                                    kAudioUnitScope_Input, 0, &fmt, sizeof(fmt)),
               "Couldn't set input format");

    checkError(AudioUnitInitialize(renderer->outputUnit),
               "Could not initialize output unit");

    renderer->sampleRate = getStreamSampleRate(renderer->outputUnit, kAudioUnitScope_Input);

    printf("Sample rate: %.0f Hz\n", renderer->sampleRate);

    AudioDeviceID dev = 0;
    UInt32 sz = sizeof(dev);
    AudioUnitGetProperty(renderer->outputUnit, kAudioOutputUnitProperty_CurrentDevice,
                         kAudioUnitScope_Global, 0, &dev, &sz);

    AudioObjectPropertyAddress addr = { kAudioDevicePropertyBufferFrameSize,
                                        kAudioObjectPropertyScopeGlobal,
                                        kAudioObjectPropertyElementMain };

    UInt32 frames = 256;  // request; optional
    AudioObjectSetPropertyData(dev, &addr, 0, nullptr, sizeof(frames), &frames);

    sz = sizeof(frames);
    AudioObjectGetPropertyData(dev, &addr, 0, nullptr, &sz, &frames);
    printf("Buffer size: %u frames\n", frames);
}

//==========================================
//Render function to place in main
//==========================================
void renderAudio(MyRenderer* renderer)
{
    renderer->outputUnit = nullptr;
    renderer->startingFrameCount = 0.0;
    renderer->outputData.store(0.0f, std::memory_order_relaxed);
    //============================
    //Callback function
    //============================
    CreateAndConnectOutputUnit(renderer); //Callback function, good here
    //============================
    //Start playback
    //============================
    checkError(AudioOutputUnitStart(renderer->outputUnit),
               "Could not start output unit");
}

void stopAudio(MyRenderer* renderer)
{
    printf("Stopping");
    AudioOutputUnitStop(renderer->outputUnit);
    AudioUnitUninitialize(renderer->outputUnit);
    AudioComponentInstanceDispose(renderer->outputUnit);
}

void getData(MyRenderer* renderer)
{
    std::cout << &renderer->outputData << std::endl;
}
