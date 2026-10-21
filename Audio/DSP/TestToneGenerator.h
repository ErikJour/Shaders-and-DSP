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
#include "AnimatedNoise.h"
#include "AnimatedGain.h"

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

struct Oscillator {

    double phase = 0.0;
    double j = 0.0;
    double cycleLength = 0.0;

    void setJ(double mJ)
    {
        j = mJ;
    }

    void prepareOscillator(MyRenderer *renderer, float frequency)
    {
        j           = renderer->startingFrameCount;
        cycleLength = renderer->sampleRate / frequency;
    }

    void setPhase()
    {
        phase                 = j / cycleLength;
    }

    float nextValue()
    {
        return (Float32) sin(2 * M_PI * phase);
    }

    void incrementPhase(MyRenderer *renderer)
    {
        j += 1.0;
        renderer->currentSample++;
        if (j > cycleLength) { j -= cycleLength; }
    }
};









//======================================================================================
inline void renderVoice(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {

    double j = 0.0;

    auto *leftChannel  = (Float32 *) ioData->mBuffers[0].mData;
    auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;
    NoiseGenerator noise;
    Oscillator     oscillator;
    oscillator.setJ(j);
    oscillator.prepareOscillator(renderer, 400.0);


    for (int frame = 0; frame < inNumberFrames; ++frame) {

        oscillator.setPhase();
        float oscSample       = oscillator.nextValue();
        float noiseSample     = noise.nextValue();

        (leftChannel)[frame]  = oscSample + noiseSample * applyGain(0.5f);
        (rightChannel)[frame] = oscSample + noiseSample * applyGain(0.5f);
        setOutputData(renderer, *leftChannel);
        oscillator.incrementPhase(renderer);


    }

    updateOutData(renderer);

    renderer->startingFrameCount = oscillator.j;

}
