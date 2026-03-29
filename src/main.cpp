#include <iostream>
#include <cmath>
#include <cstdint>
#include <portaudio.h>

constexpr int SAMPLE_RATE    = 44100;
constexpr double TWOPI       = 6.28318530718;
constexpr int FRAMES_PER_BUF = 64;

constexpr int SECS = 5;

struct Data {
    float phase; 
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
    const float freq = 440.0f;
    float phase = data->phase;

    for (int i = 0; i < frames; i++) {
        *out++ = sinf(phase);
        phase += TWOPI * freq / SAMPLE_RATE;

        if (phase >= TWOPI) {
            phase -= TWOPI;
        }
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
    Data data = { 0.0f };

    err = Pa_Initialize();
    if (err != paNoError) {
        goto out_err; 
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

    std::cout << "Outputing a 440hz sine wave for " << SECS << "seconds\n";

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
        goto out_err;

    Pa_SetStreamFinishedCallback(stream, streamFinished);
    err = Pa_StartStream(stream);
    if (err != paNoError)
        goto out_err;

    Pa_Sleep(SECS * 1000);

    err = Pa_StopStream(stream);
    if (err != paNoError)
        goto out_err;

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        goto out_err;

    Pa_Terminate();
    std::cout << "finished!" << std::endl;
    return 0;

out_err:
    Pa_Terminate();
    std::cerr << "An Error Occured!" << std::endl;
    return 1;
}
