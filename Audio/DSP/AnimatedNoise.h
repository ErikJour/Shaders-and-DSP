//
// Created by Erik Jourgensen on 10/20/26.
//

#pragma once

class NoiseGenerator
{
public:
    void reset()
    {
        noiseSeed = 22222;
    }

    void processBufferNoise(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {
        int frame          = 0;
        auto *leftChannel  = (Float32 *) ioData->mBuffers[0].mData;
        auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;

        for (frame = 0; frame < inNumberFrames; ++frame) {
            auto sample     = nextValue();
            (leftChannel)[frame]  += sample;
            (rightChannel)[frame] += sample;

        }
    }



    float nextValue() {
    // Generate the next integer pseudorandom number.
        noiseSeed = noiseSeed * 196314165 + 907633515;
    // Convert to a signed value.
        int temp = int(noiseSeed >> 7) - 16777216;
    // Convert to a floating-point number between -1.0 and 1.0.
        return float(temp) / 16777216.0f;
    }
private:
    unsigned int noiseSeed;
};
