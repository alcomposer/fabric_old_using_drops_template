#include "grain.hpp"
#include <iostream>

#define MAX_GRAINS 128

struct GrainPlayer
{
    Grain grain_array[MAX_GRAINS];
    Grain grain_array_tmp[MAX_GRAINS];
    unsigned int grain_array_length = 0;

    GrainPlayer()
    {
    }

    virtual ~GrainPlayer() {
    }

    // Return number of active grains
    //virtual int size()
    //{
    //  return(grain_array_length);
    //}
//
    //virtual bool isEmpty()
    //{
    //  return(grain_array_length == 0);
    //}
//
    //void purge()
    //{
    //  grain_array_length = 0;
    //}

    void add(float start_position, unsigned int lifespan, GRAIN_DIR direction, audioSpan *sample_ptr, unsigned int max_grains)
    {
        if(grain_array_length > max_grains || (grain_array_length >= (MAX_GRAINS - 1))) {
          std::cout << "grain array length is > than max grains or equal to MAX_GRAINS" << std::endl;
          return;
          }

        if(lifespan == 0) {
          std::cout << "lifespan is zero" << std::endl;
          return;
        }
        //Grain grain;

        // Configure grain for playback
        grain_array[grain_array_length].start_position_int = start_position * sample_ptr->size();
        grain_array[grain_array_length].start_position = start_position;
        grain_array[grain_array_length].sample_ptr = sample_ptr;
        grain_array[grain_array_length].lifespan = lifespan;
        grain_array[grain_array_length].age = lifespan;
        grain_array[grain_array_length].direction = direction;


        //grain_array[grain_array_length] = grain;
        grain_array_length++;
        std::cout << "grain added successfuly, array length is: " << grain_array_length << std::endl;
    }

    virtual std::pair<float,float> process()
    {
        std::pair<float,float> mix_output = {0.0,0.0};
        //unsigned int grain_array_tmp_length = 0;

        //
        // Process grains
        // ---------------------------------------------------------------------

        for (unsigned int i=0; i < grain_array_length; i++)
        {
            if(grain_array[i].erase_me == false)
            {
                std::cout << "processing grain: " << i << std::endl;
                std::pair<float,float> output = grain_array[i].getOutput();
                mix_output.first  += output.first;
                mix_output.second += output.second;

                grain_array[i].step();

                //grain_array_tmp[grain_array_tmp_length] = grain_array[i];
                //grain_array_tmp_length++;
            } 
        }
        //std::cout << "grain array lenth: " << grain_array_length << "grain array temp lenth: " << grain_array_tmp_length << std::endl;

        //std::swap(grain_array, grain_array_tmp);
        //grain_array_length = grain_array_tmp_length;  // I suspect this is it


        return mix_output;
    }

};
