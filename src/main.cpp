#include <iostream>
#include <cmath>
#include <cstdint>
#include <portaudio.h>
#include <csignal>

constexpr int SAMPLE_RATE     = 48000;
constexpr int SAMPLES_PER_BIT = 480;
constexpr double TWOPI        = 6.28318530718;
constexpr int FRAMES_PER_BUF  = 64;

struct Data {
    float phase; 
    int   samples_in_bit;
    int   current_freq;
};

static int paCallback(const void *input,
                      void *output,
                      long unsigned int frames,
                      const PaStreamCallbackTimeInfo *ti,
                      PaStreamCallbackFlags fl,
                      void *userData)
{
    float *out = (float*)output;
    Data *data = (Data*)userData;
    float phase = data->phase;

    for (int i = 0; i < frames; i++) {
        if (data->samples_in_bit == 0) {
            data->samples_in_bit = SAMPLES_PER_BIT;
            data->current_freq = (data->current_freq == 1000) ? 2000 : 1000;
        }

        *out++ = sinf(phase);
        phase += TWOPI * data->current_freq / SAMPLE_RATE;

        if (phase >= TWOPI) {
            phase -= TWOPI;
        }
    
        data->samples_in_bit--;
    }
    
    data->phase = phase;
    return paContinue;
}

static void streamFinished(void * userData)
{
    std::cout << "Stream finished!\n";
}

int main() 
{
    PaStreamParameters outputParams;
    PaStream *stream;
    PaError err;
    Data data = { 0.0f, 0, 1000 };

    err = Pa_Initialize();
    if (err != paNoError) {
        return 1; 
    }

    outputParams.device = Pa_GetDefaultOutputDevice();

    if (outputParams.device == paNoDevice) {
        std::cerr << "No default output device.\n";
        return 1;
    }

    outputParams.channelCount = 1;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

    std::cout << "Outputing an alternating sine wave\n";

    Pa_OpenStream(
        &stream,
        NULL,
        &outputParams,
        SAMPLE_RATE,
        FRAMES_PER_BUF,
        paClipOff,
        paCallback,
        &data
    );

    if (err != paNoError)
        return 1;

    Pa_SetStreamFinishedCallback(stream, streamFinished);
    err = Pa_StartStream(stream);
    if (err != paNoError)
        return 1;

    Pa_Sleep(10 * 1000);

    err = Pa_StopStream(stream);
    if (err != paNoError)
        return 1;

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        return 1;

    Pa_Terminate();
    std::cout << "finished!" << std::endl;
    return 0;
}
