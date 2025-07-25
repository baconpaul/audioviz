/*
 * AudioViz - just baconpaul noodling on stuff
 *
 * Copyright 2024, Paul Walker. Released under the MIT license.
 *
 * The images in the 'res' folder may be copyrighted and released
 * under restricted license. The code in scripts/ and src/ is all
 * MIT. But really, nothing to see here. Just a collaboration on
 * SFML for my current band
 *
 * All source for is available at
 * https://github.com/baconpaul/audioviz
 */

#include "audio.h"

#include <cmath>
#include "RtAudio.h"

#include "infra/glog.h"
#include "sst/basic-blocks/simd/setup.h"
#include "sst/basic-blocks/dsp/FastMath.h"
#include <complex>

namespace audioviz::audio
{

void rtaErrorCallback(RtAudioErrorType errorType, const std::string &errorText)
{
    GLOG("[ERROR] RtAudio reports '" << errorText << "'"
                                     << " " << errorType);
}

int rtaCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                double /* streamTime */, RtAudioStreamStatus status, void *data)
{
    if (status)
    {
    }

    auto asys = (AudioSystem *)data;
    asys->process(inputBuffer, nBufferFrames);

    return 0;
}

#define ADD(a, b) SIMD_MM(add_ps)(a, b)
#define SUB(a, b) SIMD_MM(sub_ps)(a, b)
#define DIV(a, b) SIMD_MM(div_ps)(a, b)
#define MUL(a, b) SIMD_MM(mul_ps)(a, b)
#define SETALL(a) SIMD_MM(set1_ps)(a)

struct CytomicSVF
{
    SIMD_M128 ic1eq{SIMD_MM(setzero_ps)()}, ic2eq{SIMD_MM(setzero_ps)()};
    SIMD_M128 g, k, gk, a1, a2, a3, m0, m1, m2;

    SIMD_M128 oneSSE{SETALL(1.0)};
    SIMD_M128 negoneSSE{SETALL(-1.0)};
    SIMD_M128 twoSSE{SETALL(2.0)};
    SIMD_M128 negtwoSSE{SETALL(-2.0)};
    enum Mode
    {
        LP,
        HP,
        BP,
        NOTCH,
        PEAK,
        ALL,
        BELL,
        LOW_SHELF,
        HIGH_SHELF
    };

    /*
     * Mode per above
     * freq is frequency in hz
     * Resonance is 0...1 resonance
     * srInv is the inverse sample rate
     * bellShelfAmp is only used in BELL/LOW_SHELF/HIGH_SHELF for the amplitude.
     * The cytomic documents use 10^db/40 scale but here we assume the pow happens
     * outside throught some other means
     */
    void setCoeff(Mode mode, float freq, float res, float srInv, float bellShelfAmp = 1.f)
    {
        auto conorm = std::clamp(freq * srInv, 0.f, 0.499f); // stable until nyquist
        res = std::clamp(res, 0.f, 0.98f);
        bellShelfAmp = std::max(bellShelfAmp, 0.001f);

        g = SETALL(sst::basic_blocks::dsp::fasttan(M_PI * conorm));
        k = SETALL(2.0 - 2 * res);

        if (mode == BELL)
        {
            k = DIV(k, SETALL(bellShelfAmp));
        }
        setCoeffPostGK(mode, SETALL(bellShelfAmp));
    }

    void setCoeff(Mode mode, float freqL, float freqR, float resL, float resR, float srInv,
                  float bellShelfAmpL, float bellShelfAmpR)
    {
        auto coL = M_PI * std::clamp(freqL * srInv, 0.f, 0.499f); // stable until nyquist
        auto coR = M_PI * std::clamp(freqR * srInv, 0.f, 0.499f); // stable until nyquist
        g = sst::basic_blocks::dsp::fasttanSSE(SIMD_MM(set_ps)(0, 0, coR, coL));
        auto res =
            SIMD_MM(set_ps)(0, 0, std::clamp(resR, 0.f, 0.98f), std::clamp(resL, 0.f, 0.98f));

        auto bellShelfAmp =
            SIMD_MM(set_ps)(0, 0, std::max(bellShelfAmpL, 0.001f), std::max(bellShelfAmpR, 0.001f));

        k = SUB(twoSSE, MUL(twoSSE, res));
        if (mode == BELL)
        {
            k = DIV(k, bellShelfAmp);
        }
        setCoeffPostGK(mode, bellShelfAmp);
    }

    void setCoeffPostGK(Mode mode, SIMD_M128 bellShelfSSE)
    {
        gk = ADD(g, k);
        a1 = DIV(oneSSE, ADD(oneSSE, MUL(g, gk)));
        a2 = MUL(g, a1);
        a3 = MUL(g, a2);

        switch (mode)
        {
        case LP:
            m0 = SIMD_MM(setzero_ps)();
            m1 = SIMD_MM(setzero_ps)();
            m2 = oneSSE;
            break;
        case BP:
            m0 = SIMD_MM(setzero_ps)();
            m1 = oneSSE;
            m2 = SIMD_MM(setzero_ps)();
            break;
        case HP:
            m0 = oneSSE;
            m1 = SUB(SIMD_MM(setzero_ps)(), k);
            m2 = negoneSSE;
            break;
        case NOTCH:
            m0 = oneSSE;
            m1 = SUB(SIMD_MM(setzero_ps)(), k);
            m2 = SIMD_MM(setzero_ps)();
            break;
        case PEAK:
            m0 = oneSSE;
            m1 = SUB(SIMD_MM(setzero_ps)(), k);
            m2 = negtwoSSE;
            break;
        case ALL:
            m0 = oneSSE;
            m1 = MUL(negtwoSSE, k);
            m2 = SIMD_MM(setzero_ps)();
            break;
        case BELL:
        {
            auto A = bellShelfSSE;
            m0 = oneSSE;
            m1 = MUL(k, SUB(MUL(A, A), oneSSE));
            m2 = SIMD_MM(setzero_ps)();
        }
        break;
        case LOW_SHELF:
        {
            auto A = bellShelfSSE;
            m0 = oneSSE;
            m1 = MUL(k, SUB(A, oneSSE));
            m2 = SUB(MUL(A, A), oneSSE);
        }
        break;
        case HIGH_SHELF:
        {
            auto A = bellShelfSSE;
            m0 = MUL(A, A);
            m1 = MUL(MUL(k, SUB(oneSSE, A)), A);
            m2 = SUB(oneSSE, MUL(A, A));
        }
        break;
        default:
            m0 = SIMD_MM(setzero_ps)();
            m1 = SIMD_MM(setzero_ps)();
            m2 = SIMD_MM(setzero_ps)();
            break;
        }
    }

    void fetchCoeffs(const CytomicSVF &that)
    {
        g = that.g;
        k = that.k;
        gk = that.gk;
        a1 = that.a1;
        a2 = that.a2;
        a3 = that.a3;
        da1 = that.da1;
        da2 = that.da2;
        da3 = that.da3;
        m0 = that.m0;
        m1 = that.m1;
        m2 = that.m2;
    }

    void fetchCoeffsAtIndex(const CytomicSVF &that, int index)
    {
        g[index] = that.g[0];
        k[index] = that.k[0];
        gk[index] = that.gk[0];
        a1[index] = that.a1[0];
        a2[index] = that.a2[0];
        a3[index] = that.a3[0];
        da1[index] = that.da1[0];
        da2[index] = that.da2[0];
        da3[index] = that.da3[0];
        m0[index] = that.m0[0];
        m1[index] = that.m1[0];
        m2[index] = that.m2[0];
    }

    static void step(CytomicSVF &that, float &L, float &R)
    {
        auto vin = SIMD_MM(set_ps)(0, 0, R, L);
        auto res = stepSSE(that, vin);
        float r4 alignas(16)[4];
        SIMD_MM(store_ps)(r4, res);
        L = r4[0];
        R = r4[1];
    }

    static SIMD_M128 stepSSE(CytomicSVF &that, SIMD_M128 vin)
    {
        // v3 = v0 - ic2eq
        auto v3 = SUB(vin, that.ic2eq);

        // v1 = a1 * ic1eq + a2 * v3
        auto v1 = ADD(MUL(that.a1, that.ic1eq), MUL(that.a2, v3));

        // v2 = ic2eq + a2 * ic1eq + a3 * v3
        auto v2 = ADD(that.ic2eq, ADD(MUL(that.a2, that.ic1eq), MUL(that.a3, v3)));

        // ic1eq = 2 * v1 - ic1eq
        that.ic1eq = SUB(MUL(that.twoSSE, v1), that.ic1eq);

        // ic2eq = 2 * v2 - ic2eq
        that.ic2eq = SUB(MUL(that.twoSSE, v2), that.ic2eq);

        // (m0 * input) + ((m1 * v1) + (m2 * v2))
        return ADD(MUL(that.m0, vin), ADD(MUL(that.m1, v1), MUL(that.m2, v2)));
    }

    /*
     * Process across a block with smoothing
     */
    SIMD_M128 da1, da2, da3;
    SIMD_M128 dm0, dm1, dm2;
    bool firstBlock{true};

    template <int blockSize>
    void setCoeffForBlock(Mode mode, float freq, float res, float srInv, float bellShelfAmp = 1.f)
    {
        // Preserve the prior values
        SIMD_M128 a1_prior = a1;
        SIMD_M128 a2_prior = a2;
        SIMD_M128 a3_prior = a3;

        SIMD_M128 m0_prior = m0;
        SIMD_M128 m1_prior = m1;
        SIMD_M128 m2_prior = m2;

        // calculate the new values
        setCoeff(mode, freq, res, srInv, bellShelfAmp);

        // If its the first time around snap them
        if (firstBlock)
        {
            a1_prior = a1;
            a2_prior = a2;
            a3_prior = a3;
            m0_prior = m0;
            m1_prior = m1;
            m2_prior = m2;
            firstBlock = false;
        }

        // then for each one calculate the change across the block
        static constexpr float obsf = 1.f / blockSize;
        auto obs = SETALL(obsf);

        // and set the changeup, and reset as to the prior value so we move in the block
        da1 = MUL(SUB(a1, a1_prior), obs);
        a1 = a1_prior;

        da2 = MUL(SUB(a2, a2_prior), obs);
        a2 = a2_prior;

        da3 = MUL(SUB(a3, a3_prior), obs);
        a3 = a3_prior;

        dm0 = MUL(SUB(m0, m0_prior), obs);
        m0 = m0_prior;

        dm1 = MUL(SUB(m1, m1_prior), obs);
        m1 = m1_prior;

        dm2 = MUL(SUB(m2, m2_prior), obs);
        m2 = m2_prior;
    }

    // it's a bit annoying this is a copy but I am sure a clever future me will do better
    template <int blockSize>
    void setCoeffForBlock(Mode mode, float freqL, float freqR, float resL, float resR, float srInv,
                          float bellShelfAmpL, float bellShelfAmpR)
    {
        SIMD_M128 a1_prior = a1;
        SIMD_M128 a2_prior = a2;
        SIMD_M128 a3_prior = a3;

        SIMD_M128 m0_prior = m0;
        SIMD_M128 m1_prior = m1;
        SIMD_M128 m2_prior = m2;

        setCoeff(mode, freqL, freqR, resL, resR, srInv, bellShelfAmpL, bellShelfAmpR);

        if (firstBlock)
        {
            a1_prior = a1;
            a2_prior = a2;
            a3_prior = a3;
            m0_prior = m0;
            m1_prior = m1;
            m2_prior = m2;
            firstBlock = false;
        }

        static constexpr float obsf = 1.f / blockSize;
        auto obs = SETALL(obsf);

        da1 = MUL(SUB(a1, a1_prior), obs);
        a1 = a1_prior;

        da2 = MUL(SUB(a2, a2_prior), obs);
        a2 = a2_prior;

        da3 = MUL(SUB(a3, a3_prior), obs);
        a3 = a3_prior;

        dm0 = MUL(SUB(m0, m0_prior), obs);
        m0 = m0_prior;

        dm1 = MUL(SUB(m1, m1_prior), obs);
        m1 = m1_prior;

        dm2 = MUL(SUB(m2, m2_prior), obs);
        m2 = m2_prior;
    }

    template <int blockSize> void retainCoeffForBlock()
    {
        da1 = SIMD_MM(setzero_ps)();
        da2 = SIMD_MM(setzero_ps)();
        da3 = SIMD_MM(setzero_ps)();
        dm0 = SIMD_MM(setzero_ps)();
        dm1 = SIMD_MM(setzero_ps)();
        dm2 = SIMD_MM(setzero_ps)();
    }

    void processBlockStep(float &L, float &R)
    {
        step(*this, L, R);
        a1 = ADD(a1, da1);
        a2 = ADD(a2, da2);
        a3 = ADD(a3, da3);
        m1 = ADD(m1, dm1);
        m2 = ADD(m2, dm2);
        m0 = ADD(m0, dm0);
    }

    void processBlockStep(float &L)
    {
        float tmp{0.f};
        step(*this, L, tmp);
        a1 = ADD(a1, da1);
        a2 = ADD(a2, da2);
        a3 = ADD(a3, da3);
        m1 = ADD(m1, dm1);
        m2 = ADD(m2, dm2);
        m0 = ADD(m0, dm0);
    }

    template <int blockSize>
    void processBlock(const float *const inL, const float *const inR, float *outL, float *outR)
    {
        for (int i = 0; i < blockSize; ++i)
        {
            outL[i] = inL[i];
            outR[i] = inR[i];
            processBlockStep(outL[i], outR[i]);
        }
    }

    template <int blockSize> void processBlock(const float *const inL, float *outL)
    {
        for (int i = 0; i < blockSize; ++i)
        {
            outL[i] = inL[i];
            processBlockStep(outL[i]);
        }
    }

    void init()
    {
        ic1eq = SIMD_MM(setzero_ps)();
        ic2eq = SIMD_MM(setzero_ps)();
    }

#undef ADD
#undef SUB
#undef DIV
#undef MUL
#undef SETALL
};


AudioSystem::AudioSystem()
{
    session = std::make_unique<RtAudio>(RtAudio::UNSPECIFIED, &rtaErrorCallback);
    session->showWarnings(true);
    selectedDevice = defaultInputDevice();
}
AudioSystem::~AudioSystem() = default;

std::vector<AudioSystem::device_t> AudioSystem::inputDevices()
{
    GLOG("Listing input devices");
    auto &audio = *session;
    // Get the list of device IDs
    auto ids = audio.getDeviceIds();
    if (ids.size() == 0)
    {
        GLOG("No devices found.");
        return {};
    }

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo info;
    std::vector<AudioSystem::device_t> res;
    for (unsigned int n = 0; n < ids.size(); n++)
    {
        info = audio.getDeviceInfo(ids[n]);
        if (info.inputChannels > 0)
        {
            res.push_back({n, info.name});
        }
    }
    return res;
}

AudioSystem::device_t AudioSystem::defaultInputDevice()
{
    auto &audio = *session;
    // Get the list of device IDs
    std::vector<unsigned int> ids = audio.getDeviceIds();
    if (ids.size() == 0)
    {
        GLOG("No devices found.");
        return {};
    }

    // Scan through devices for various capabilities
    RtAudio::DeviceInfo info;
    for (unsigned int n = 0; n < ids.size(); n++)
    {
        info = audio.getDeviceInfo(ids[n]);

        if (info.isDefaultInput)
        {
            return {n, info.name};
        }
    }
    return {};
}

void AudioSystem::selectInput(int idx)
{
    auto &audio = *session;
    // Get the list of device IDs
    std::vector<unsigned int> ids = audio.getDeviceIds();
    auto theInfo = audio.getDeviceInfo(ids[idx]);
    selectedDevice = {idx, theInfo.name};
}

void AudioSystem::start()
{
    auto &audio = *session;
    std::vector<unsigned int> ids = audio.getDeviceIds();
    auto theInfo = audio.getDeviceInfo(ids[selectedDevice.first]);

    auto sr = theInfo.preferredSampleRate;

    GLOG("Starting audio " << theInfo.name << " at " << sr << " with " << theInfo.inputChannels
                           << " input channels");

    auto audioInputDeviceID = ids[selectedDevice.first];
    auto audioInputUsed = true;
    auto audioOutputUsed = false;

    auto dids = session->getDeviceIds();
    auto dnms = session->getDeviceNames();

    RtAudio::StreamParameters oParams;
    uint32_t sampleRate{sr};

    RtAudio::StreamParameters iParams;
    iParams.deviceId = audioInputDeviceID;
    auto inInfo = session->getDeviceInfo(iParams.deviceId);
    iParams.nChannels = std::min(2U, inInfo.inputChannels);
    nChannels = iParams.nChannels;
    iParams.firstChannel = 0;
    if (sampleRate < 0)
        sampleRate = inInfo.preferredSampleRate;

    if (sampleRate < 0)
    {
        GLOG("No preferred sample rate detected; using 48k");
        sampleRate = 48000;
    }

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_SCHEDULE_REALTIME;

    uint32_t bufferFrames{256};
    if (session->openStream(nullptr, &iParams, RTAUDIO_FLOAT32, sampleRate, &bufferFrames,
                            &rtaCallback, (void *)this, &options))
    {
        GLOG("[ERROR]" << session->getErrorText());
        session->closeStream();
        return;
    }

    if (!session->isStreamOpen())
    {
        GLOG("[ERROR] Stream failed to open : " << session->getErrorText());
        return;
    }

    if (session->startStream())
    {
        GLOG("[ERROR] startStream failed : " << session->getErrorText());
        return;
    }

    GLOG("RtAudio: Started Stream");


    float freq0{80.f};
    float dFreq{std::cbrt(2.f)};
    for (int i=0; i<nFilters; ++i)
    {
        filter[i] = std::make_unique<CytomicSVF>();
        filter2[i] = std::make_unique<CytomicSVF>();
        for (int j=0; j<bandsPerFilter; ++j)
        {
            CytomicSVF tmp;
            auto mode = CytomicSVF::BP;
            auto res = 0.7f;

            GLOG("Creating filter at " << freq0 << " / " << res);
            freq0 *= dFreq;
            tmp.setCoeff(mode, freq0, res, 1.0 / sampleRate);
            filter[i]->fetchCoeffsAtIndex(tmp, j);
        }
        filter2[i]->fetchCoeffs(*filter[i]);
    }
}

void AudioSystem::stop()
{
    if (!isRunning())
        return;
    session->stopStream();
    session->closeStream();
}

bool AudioSystem::isRunning() const { return session->isStreamRunning(); }

void AudioSystem::process(void *inData, int nFrames)
{
    auto *d = (float *)inData;
    // For now only use one channel
    for (int i = 0; i < nFrames * nChannels; i += nChannels)
    {
        auto nv = std::max(std::fabs(d[i]), level.load()) * 0.9995f;
        level = nv;
        auto q = SIMD_MM(set1_ps)(d[i]);

        for (int j=0; j<nFilters; ++j)
        {
            auto out = CytomicSVF::stepSSE(*filter[j], q);
            out = CytomicSVF::stepSSE(*filter2[j], out);
            for (int k=0; k<bandsPerFilter; ++k)
            {
                auto nv = std::max(std::fabs(out[k]), bands[j*4+k].load()) * 0.99995f;
                bands[j*bandsPerFilter+k] = nv;
            }
        }
    }
}
} // namespace audioviz::audio