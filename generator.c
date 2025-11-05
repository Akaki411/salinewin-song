#include "generator.h"

void generate_bytebeat(unsigned char *buffer, int numSamples, int samplerate, int duration) 
{
    int time, currentState = 0;
    int starting = 8;
    for (int i = currentState; i < samplerate * starting; i++) 
    {
        time = i / 12;
        currentState = i;
        int result = time * ((time / 2 >> 10 | time % 16 * time >> 8) & 8 * time >> 12 & 18) | (0 - (time / 16)) + 64;
        buffer[i] = (unsigned char)(result & 0xFF);
    }

    for (int i = currentState; i < samplerate * (duration - starting); i++) 
    {
        time = i / 12;
        currentState = i;
        int result = time * ((time & 4096 ? (time % 65536 < 59392 ? 7 : time & 7) : 16) ^ (1 & (time >> 14))) >> (3 & -time >> (time & 2048 ? 2 : 10));
        buffer[i] = (unsigned char)(result & 0xFF);
    }
}