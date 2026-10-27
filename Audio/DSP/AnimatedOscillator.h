//
// Created by Erik Jourgensen on 10/27/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "../AudioUtilities.h"
#include "../AudioTypes.h"

//======================================================================================

struct Oscillator {

    double phase = 0.0;
    double increment = 0.0;

    void prepareOscillator(double sampleRate, double frequency)
    {
        increment = frequency / sampleRate;
    }

    float nextValue()
    {
        auto sine = (Float32) sin(2 * M_PI * phase);
        phase += increment;
        if (phase >- 1.0) { phase -= 1.0; }
        return sine;
    }

};
