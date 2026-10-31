//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <atomic>
#import  "DSP/data/AnimatedFFT.h"
#include "DSP/generators/AnimatedOscillator.h"
#include "DSP/generators/AnimatedNoise.h"
#include "DSP/effects/AnimatedDelay.h"
#include "DSP/shapers/AnimatedEnvelope.h"
#include "DSP/data/AudioData.h"
#include "DSP/effects/AnimatedGain.h"


struct MyRenderer
{
    AudioUnit           outputUnit{nullptr};
    double              startingFrameCount{0.0};
    double              sampleRate {};
    AnimatedFFT         fft[2];
    UInt32              currentSample{};
    Oscillator          osc{};
    NoiseGenerator      noise{};
    AnimatedDelay       delay{};
    AnimatedEnvelope    env{};
    AudioData           data{};
    AnimatedGain        gain{};

    void prepareToPlay(double newSampleRate, UInt32 maxFrames)
    {
        sampleRate = newSampleRate;
        startingFrameCount = 0.0f;
        fft[0].reset();
        fft[1].reset();
    }
};