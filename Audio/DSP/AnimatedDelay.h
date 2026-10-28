//
// Created by Erik Jourgensen on 10/28/26.
//

#pragma once
#include <iostream>
#include <ostream>
#include <vector>

#define MAX_DELAY_SECONDS 3


struct AnimatedDelay {

    int                      mDelayBufferLength = 0;
    std::unique_ptr<float[]> mDelayBuffer         {};
    int                      mDelayInSamples    = 0;
    int                      mWritePosition     = 0;
    int                      mReadPosition      = 0;
    float                    currentValue       = 0.0f;

    void init (const double sampleRate)
    {
        mDelayBufferLength = static_cast<int>(sampleRate) * MAX_DELAY_SECONDS;
        mDelayBuffer       = std::make_unique<float[]>(static_cast<size_t>(mDelayBufferLength));
        mWritePosition     = 0;
    }

    void setDelay(int delayInSamples)
    {
        if (delayInSamples < 0 )                      { delayInSamples = 0;                      }
        if (delayInSamples > mDelayBufferLength - 1 ) { delayInSamples = mDelayBufferLength - 1; }
        mDelayInSamples = delayInSamples;
        mReadPosition   = mReadPosition - mDelayInSamples;
        if ( mReadPosition < 0 )        { mReadPosition += mDelayBufferLength; }
    }

    float processSample(float input)
    {
        const float inputSample                                 = input;
        mDelayBuffer[static_cast<size_t>(mWritePosition)]       = inputSample;
        const float outputSample                                = mDelayBuffer[static_cast<size_t>(mReadPosition)];
        //======================================================================================================
        //Feedback Loop
        //======================================================================================================
        mDelayBuffer[static_cast<size_t>(mWritePosition)]       = inputSample + outputSample;
        input                                                   = (inputSample * 0.5f) + (outputSample * 0.5f);
        //======================================================================================================
        mWritePosition++;
        if (mWritePosition >= mDelayBufferLength) { mWritePosition = 0; }
        mReadPosition++;
        if (mReadPosition >= mDelayBufferLength) { mReadPosition = 0; }
        if (mReadPosition <= 22100 && mReadPosition >= 0) {
            currentValue = outputSample;
        }
        return outputSample;
    }


    void clearBuffer()
    {
        for (int i = 0; i < mDelayBufferLength; i++) {
            mDelayBuffer[i] = 0.0f;
        }
    }

};






