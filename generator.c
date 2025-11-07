#include "generator.h"
#include <stdio.h>

typedef int (*SampleProcessor)(int t);

struct Sample
{
    SampleProcessor processor;
    double duration;
};

int overflow(int t)
{
    return t & t >> 8;
}

int pulse(int t)
{
   return (t >> 2) * (t >> 5) | t >> 5;
}

int pulse2(int t)
{
    return 2 * (t >> 5 & t) - (t >> 5) + t * (t >> 14 & 14);
}

int rising(int t)
{
    return t * ((t / 2 >> 10 | t % 16 * t >> 8) & 8 * t >> 12 & 18) | (0 - (t / 16)) + 64;
}

int main_part(int t)
{
    return t * ((t & 4096 ? (t % 65536 < 59392 ? 7 : t & 7) : 16) ^ (1 & (t >> 14))) >> (3 & -t >> (t & 2048 ? 2 : 10));
}

int ending(int t)
{
    return t * (t ^ t + (t >> 15 | 1) ^ (t - 1280 ^ t) >> 10);
}

int current_sample(struct Sample *sample, int count, double time)
{
    double total_duration = 0;

    for (int i = 0; i < count; i++)
    {
        total_duration += sample[i].duration;

        if (time < total_duration)
        {
            return i;
        }
    }

    return 0;
}

void generate_bytebeat(unsigned char *buffer, int samplerate, int duration)
{
    int t;
    double time;
    struct Sample samples[6] = {
        {overflow, 25.0},
        {pulse, 5.0},
        {pulse2, 30.0},
        {rising, 30.0},
        {main_part, 60.0},
        {ending, 10.0}
    };
    int count = sizeof(samples) / sizeof(samples[0]);
    for (int i = 0; i < samplerate * duration; i++)
    {
        time = i / samplerate;
        t = i / 12;
        int result = samples[current_sample(samples, count, time)].processor(t);
        buffer[i] = (unsigned char)(result & 0xFF);
    }
}