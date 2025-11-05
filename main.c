#include <windows.h>
#include <stdio.h>
#include "generator.h"

#define SAMPLE_RATE 96000
#define DURATION 60

int main() 
{
    int numSamples = SAMPLE_RATE * DURATION;
    unsigned char *buffer = (unsigned char*)malloc(numSamples * sizeof(unsigned char));
    
    if (!buffer) 
    {
        printf("Memory error\n");
        return 1;
    }
    
    generate_bytebeat(buffer, numSamples, SAMPLE_RATE, DURATION);
    
    short *buffer16 = (short*)malloc(numSamples * sizeof(short));
    if (!buffer16) 
    {
        printf("Memory allocation error for 16-bit buffer\n");
        free(buffer);
        return 1;
    }
    
    for (int i = 0; i < numSamples; i++) 
    {
        buffer16[i] = (short)((buffer[i] - 128) * 256);
    }
    
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;
    
    HWAVEOUT hWaveOut;
    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) 
    {
        printf("Sound device error\n");
        free(buffer);
        free(buffer16);
        return 1;
    }
    
    WAVEHDR waveHdr;
    waveHdr.lpData = (LPSTR)buffer16;
    waveHdr.dwBufferLength = numSamples * sizeof(short);
    waveHdr.dwFlags = 0;
    waveHdr.dwLoops = 0;
    
    if (waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) 
    {
        printf("Header error\n");
        waveOutClose(hWaveOut);
        free(buffer);
        free(buffer16);
        return 1;
    }
    
    if (waveOutWrite(hWaveOut, &waveHdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) 
    {
        printf("Playing error\n");
        waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(WAVEHDR));
        waveOutClose(hWaveOut);
        free(buffer);
        free(buffer16);
        return 1;
    }

    while (!(waveHdr.dwFlags & WHDR_DONE)) 
    {
        Sleep(100);
    }
    
    waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
    free(buffer);
    free(buffer16);
    
    return 0;
}