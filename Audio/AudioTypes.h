//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <atomic>
#import "DSP/AnimatedFFT.h"
#include "DSP/AnimatedOscillator.h"
#include "DSP/AnimatedNoise.h"
#include "DSP/AnimatedDelay.h"
#include "DSP/AnimatedEnvelope.h"

struct MyRenderer
{
    AudioUnit           outputUnit{nullptr};
    double              startingFrameCount{0.0};
    std::atomic<float>  outputData{0.0f};
    double              sampleRate {};
    AnimatedFFT         fft[2];
    UInt32              currentSample{};
    Oscillator          osc{};
    NoiseGenerator      noise{};
    AnimatedDelay       delay{};
    AnimatedEnvelope    env{};


    void prepareToPlay(double newSampleRate, UInt32 maxFrames)
    {
        sampleRate = newSampleRate;
        startingFrameCount = 0.0f;
        fft[0].reset();
        fft[1].reset();
    }
};