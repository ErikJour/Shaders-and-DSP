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


inline void generateTestTone(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames, float frequency) {
    double j           = renderer->startingFrameCount;
    double cycleLength = renderer->sampleRate / frequency;
    int frame          = 0;
    auto *leftChannel  = (Float32 *) ioData->mBuffers[0].mData;
    auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;
    NoiseGenerator noise;


    double phase = 0.0;

    for (frame = 0; frame < inNumberFrames; ++frame) {
        phase                 = j / cycleLength;
        auto sample           = (Float32) sin(2 * M_PI * phase);
        float noiseSample     = noise.nextValue();

        (leftChannel)[frame]  = sample + noiseSample * applyGain(0.5f);
        (rightChannel)[frame] = sample + noiseSample * applyGain(0.5f);
        renderer->outputData = *leftChannel;

        j += 1.0;
        renderer->currentSample++;

        if (j > cycleLength) { j -= cycleLength; }


    }
    float current = renderer->outputData.load(std::memory_order_relaxed);

    renderer->outputData.store(current, std::memory_order_relaxed);

    renderer->startingFrameCount = j;



}
