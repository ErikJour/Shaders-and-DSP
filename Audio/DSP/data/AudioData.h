//
// Created by Erik Jourgensen on 10/29/26.
//

#pragma once
#include <atomic>

struct AudioData {

    std::atomic<float>  outputData{0.0f};

    inline void setOutputData(float sampleValue)
    {
        outputData = sampleValue;
    }

    inline void updateOutData()
    {
        float current = outputData.load(std::memory_order_relaxed);

        outputData.store(current, std::memory_order_relaxed);
    }

    void getData()
    {
        std::cout << outputData << std::endl;
    }

};

