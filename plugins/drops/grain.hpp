#pragma once

#include <vector>
#include <math.h>
#include <utility>
#include <span>

#define  PI   3.141592653589793

enum GRAIN_DIR {forward,reverse};

typedef std::vector<std::pair<float,float>> audioSpan;

inline int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

struct Grain
{
    // Start Position is the offset into the sample where playback should start.
    float start_position;
    int start_position_int;

    // Playback length for the grain, measuring in .. er.. ticks?
    float playback_length;

    // sample_ptr points to the loaded sample in memory
    audioSpan *sample_ptr;

    std::vector<float> * sample_L;
    std::vector<float> * sample_R;

    // playback_position is similar to samplePos used in for samples.  However,
    // it's relative to the Grain's start_position rather than the sample
    // start position.
    int playback_position = 0;
    float pan = 0;
    int sample_position = 0;
    unsigned int age = 0;
    unsigned int lifespan = 0;
    float pitch = 0;
    bool erase_me = true;
    GRAIN_DIR direction = GRAIN_DIR::forward;

    Grain()
    {
    }

    std::pair<float,float> getOutput()
    {
        int bufferSize = sample_ptr->size();
        if(age == 0 || (bufferSize) == 0) return {0.0,0.0};

        if(direction == GRAIN_DIR::forward){
            sample_position = (start_position_int + this->playback_position);
        }else{
            sample_position = (start_position_int + (lifespan) - this->playback_position); 
        }
        sample_position = mod(sample_position,bufferSize);

        float window = sin(PI * (float)age / (float)lifespan);
        //double i = (double)age/lifespan;
        //double r = 0.5;
        //float window = (cos(fmax(fabs((double)i - 0.5) * (2.0 / r)  - (1.0 / r - 1.0), 0.0) * PI) + 1.0) / 2.0;

        std::pair<float,float> output_sample;
        output_sample.first  = sample_ptr->at(sample_position).first  * window;
        output_sample.second = sample_ptr->at(sample_position).second * window;
        return output_sample;
    };

    void step()
    {
        if(erase_me == false)
        {
            playback_position++;
            if(! --age) erase_me = true;
        }
    };
};