//
// Created by Erik Jourgensen on 10/29/26.
//
//A simple Attack / Release envelope

#pragma once
const float SILENCE = 0.0001f;

struct AnimatedEnvelope {
    float level;

    float nextValue()
    {
        level *= 0.9999f;
        return level;
    }
};
