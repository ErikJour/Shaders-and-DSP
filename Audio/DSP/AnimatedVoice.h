//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "../AudioUtilities.h"
#include "../AudioTypes.h"
#include <iostream>
#include <stdatomic.h>
#include <cmath>
#include "AnimatedGain.h"
#include "AnimatedDelay.h"

//======================================================================================
inline void setOutputData(MyRenderer *renderer, float sampleValue)
{
    renderer->outputData = sampleValue;
}

inline void updateOutData(MyRenderer *renderer)
{
    float current = renderer->outputData.load(std::memory_order_relaxed);

    renderer->outputData.store(current, std::memory_order_relaxed);
}
//======================================================================================


//======================================================================================
inline void renderVoice(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {


    auto *leftChannel  = (Float32 *) ioData->mBuffers[0].mData;
    auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;



    for (int frame = 0; frame < inNumberFrames; ++frame) {


        float oscSample       = renderer->osc.nextValue();
        float noiseSample     = renderer->noise.nextValue();
        float sample          = (oscSample + noiseSample) * applyGain(0.5f);
        float delayedSample   = renderer->delay.processSample(sample);

        (leftChannel)[frame]  = (rightChannel)[frame] = sample + delayedSample;
        setOutputData(renderer, *leftChannel);
        renderer->currentSample++;
    }

    updateOutData(renderer);

}
