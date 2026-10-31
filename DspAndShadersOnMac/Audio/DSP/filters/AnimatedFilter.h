//
// Created by Erik Jourgensen on 10/16/26.
//

#pragma once
constexpr float PI = 3.14159265358979323846f;

struct AnimatedFilter {
    double mSampleRate = 44100.0;
    float dn1          = 0.0f;
    float mDelay       = 0.0f;

    void filter(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames)
    {
        auto *leftChannel  = (Float32 *) ioData->mBuffers[0].mData;
        auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;

        for (UInt32 i = 0; i < inNumberFrames; ++i) {

            const float input          = (leftChannel)[i];
            const float output         = feedForwardFilter(input, dn1, 0.5f, -0.7f);
            dn1                        = input;
            //=================================
            //Buffer assignment
            //=================================
            (leftChannel)[i]  = output;
            (rightChannel)[i] = output;

        }
    }

    static float feedForwardFilter(float input, float delayedInput, float a0, float a1) {

        return (input * a0) + (delayedInput * a1);

    }

};


//===========================Try====Later==================================================

//
//float filterOne(const float input)
//{
//    constexpr float freq = 10000.0f;
//    const float tan_g    = std::tan((PI * freq / static_cast<float>(mSampleRate)));
//
//    const float b0       = tan_g / (1.0f + tan_g);
//    const float b1       = tan_g / (1.0f + tan_g);
//    const float a1       = (tan_g - 1.0f) / (tan_g + 1.0f);
//
//    const float output   = b0 * input + mDelay;
//    mDelay               = b1 * input - a1 * output;
//
//    return output;
//}
//
//float onePoleIIR(const float input, float delay, const float a0, const float b1)
//{
//    return a0 * input - b1 * delay;
//}