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


float gainFactor   = 0.9f;

//======================================================================================
inline void renderVoice(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {


    auto *leftChannel  = (Float32 *) ioData->mBuffers[0].mData;
    auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;

    float silence = 0.0f;

    //if note on


    for (int frame = 0; frame < inNumberFrames; ++frame) {

        float oscSample       = renderer->osc.nextValue();
        float noiseSample     = renderer->noise.nextValue() * silence;
        float sample          = (oscSample + noiseSample) * renderer->gain.applyGain(0.5f);
        float delayedSample   = renderer->delay.processSample(sample);
        float release         = renderer->env.nextValue();

        (leftChannel)[frame]  = (rightChannel)[frame] = (sample + delayedSample) * release;
        renderer->data.setOutputData(release);
        renderer->currentSample++;
    }

    //else reset delay and oscillator

    renderer->data.updateOutData();

}
