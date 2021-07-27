#pragma once

#include "grain.hpp"
#include <iostream>

#define MAX_GRAINS 256

struct GrainPlayer
{
    Grain grain_array[MAX_GRAINS];
    Grain grain_array_tmp[MAX_GRAINS];
    unsigned int grain_array_length = 0;

    GrainPlayer(){}

    virtual ~GrainPlayer() {
    }

    void add(float start_position, unsigned int lifespan, GRAIN_DIR direction, audioSpan *sample_ptr, unsigned int max_grains)
    {
        if(lifespan == 0) {
          return;
        }

        for(int pos = 0; pos < MAX_GRAINS; pos++){
          if (grain_array[pos].erase_me == true){
            Grain grain;

            // Configure grain for playback
            grain.start_position_int = start_position * sample_ptr->size();
            grain.start_position = start_position;
            grain.sample_ptr = sample_ptr;
            grain.lifespan = lifespan;
            grain.age = lifespan;
            grain.direction = direction;
            grain.erase_me = false;

            grain_array[pos] = grain;
            return;
          };
        };
    }

    std::pair<float,float> process()
    {
        std::pair<float,float> mix_output = {0.0,0.0};
        //
        // Process grains
        // ---------------------------------------------------------------------

        for (unsigned int i=0; i < MAX_GRAINS; i++)
        {
            if(grain_array[i].erase_me == false)
            {
                std::pair<float,float> output = grain_array[i].getOutput();
                mix_output.first  += output.first;
                mix_output.second += output.second;
                grain_array[i].step();
            }
        }
        return mix_output;
    }


};
