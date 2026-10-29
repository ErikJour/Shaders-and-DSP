#pragma once
#include <Accelerate/Accelerate.h>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>

class AnimatedFFT
{
public:
    static constexpr int fftOrder = 10;
    static constexpr int fftSize  = 1 << fftOrder;      // 1024
    static constexpr int numBins  = fftSize / 2 + 1;    // 513
    static constexpr int overlap  = 4;
    static constexpr int hopSize  = fftSize / overlap;  // 256

    AnimatedFFT()
    {
        setup = vDSP_create_fftsetup(fftOrder, kFFTRadix2);
        for (int n = 0; n < fftSize; ++n)
            window[n] = 0.5f * (1.0f - std::cos(2.0f * float(M_PI) * n / fftSize));
        reset();
    }

    ~AnimatedFFT() { if (setup) vDSP_destroy_fftsetup(setup); }

    AnimatedFFT(const AnimatedFFT&)            = delete;
    AnimatedFFT& operator=(const AnimatedFFT&) = delete;

    void reset()
    {
        pos = 0;
        count = 0;
        inputFifo.fill(0.0f);
        for (auto& m : magsDb) m.store(-100.0f, std::memory_order_relaxed);
    }

    // audio thread
    void pushSamples(const float* data, uint32_t numFrames)
    {
        for (uint32_t i = 0; i < numFrames; ++i)
        {
            inputFifo[pos] = data[i];
            if (++pos == fftSize) pos = 0;
            if (++count == hopSize) { count = 0; processFrame(); }
        }
    }

    // render thread
    float getBinDb(int k) const { return magsDb[k].load(std::memory_order_relaxed); }

private:
    void processFrame()
    {
        // unroll the circular FIFO: the oldest sample is at pos
        std::memcpy(frame.data(), inputFifo.data() + pos, (fftSize - pos) * sizeof(float));
        if (pos > 0)
            std::memcpy(frame.data() + fftSize - pos, inputFifo.data(), pos * sizeof(float));

        // window
        vDSP_vmul(frame.data(), 1, window.data(), 1, frame.data(), 1, fftSize);

        // forward FFT
        DSPSplitComplex split { realp.data(), imagp.data() };
        vDSP_ctoz((const DSPComplex*) frame.data(), 2, &split, 1, fftSize / 2);
        vDSP_fft_zrip(setup, &split, 1, fftOrder, FFT_FORWARD);

        // magnitudes → dB  (2/N: full-scale Hann-windowed sine ≈ 0 dB)
        const float scale = 2.0f / fftSize;
        auto toDb = [](float m) { return 20.0f * std::log10(m + 1e-9f); };
        constexpr auto relaxed = std::memory_order_relaxed;

        magsDb[0].store(toDb(std::fabs(realp[0]) * scale * 0.5f), relaxed);            // DC
        magsDb[fftSize / 2].store(toDb(std::fabs(imagp[0]) * scale * 0.5f), relaxed);  // Nyquist
        for (int k = 1; k < fftSize / 2; ++k)
            magsDb[k].store(toDb(std::hypot(realp[k], imagp[k]) * scale), relaxed);
    }

    FFTSetup setup = nullptr;
    std::array<float, fftSize>     window{}, inputFifo{}, frame{};
    std::array<float, fftSize / 2> realp{}, imagp{};
    std::array<std::atomic<float>, numBins> magsDb;
    int pos = 0, count = 0;
};