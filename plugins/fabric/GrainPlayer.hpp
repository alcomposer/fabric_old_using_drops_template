#pragma once

#include "grain.hpp"
#include <boost/intrusive/list.hpp>
#include <iostream>

#define MAX_GRAINS 256

struct GrainPlayer
{
    using GrainList = boost::intrusive::list<Grain>;

    GrainList grains_used;
    GrainList grains_free;
    Grain grain_array[MAX_GRAINS];

    GrainPlayer() {
        for (Grain &grain : grain_array)
            grains_free.push_back(grain);
    }

    virtual ~GrainPlayer() {
    }

    void add(float start_position, unsigned int lifespan, GRAIN_DIR direction, audioSpan *sample_ptr, unsigned int max_grains)
    {
        if(lifespan == 0) {
          return;
        }

        if (grains_free.empty()) {
            return;
        }

        Grain &slot = grains_free.front();
        grains_free.pop_front();
        grains_used.push_back(slot);

        // Configure grain for playback
        Grain grain;
        grain.start_position_int = start_position * sample_ptr->size();
        grain.start_position = start_position;
        grain.sample_ptr = sample_ptr;
        grain.lifespan = lifespan;
        grain.age = lifespan;
        grain.direction = direction;
        grain.erase_me = false;
    
        slot = grain;
    }

    std::pair<float,float> process()
    {
        std::pair<float,float> mix_output = {0.0,0.0};
        //
        // Process grains
        // ---------------------------------------------------------------------

        GrainList::iterator grain_iterator = grains_used.begin();

        while (grain_iterator != grains_used.end()) {
            Grain &grain = *grain_iterator;

            std::pair<float,float> output = grain.getOutput();
            mix_output.first  += output.first;
            mix_output.second += output.second;
            grain.step();

            GrainList::iterator saved_iterator = grain_iterator++;
            if (grain.erase_me) {
                grains_used.erase(saved_iterator);
                grains_free.push_back(grain);
            }
        }
        return mix_output;
    }


};
