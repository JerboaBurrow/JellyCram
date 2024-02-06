#ifndef DISPLAY_H
#define DISPLAY_H

#include <glm/glm.hpp>
#include <chrono>
#include <thread>
#include <iostream>

namespace jGL
{
    class Display
    {
    
    public:

        Display(glm::ivec2 res)
        : 
            resolution(res), 
            unlimited(true), 
            framesPerSecond(-1),
            secondsPerFrame(0.0),
            lastFrame(std::chrono::high_resolution_clock::now())
        {}

        unsigned getResX() const { return resolution.x; }
        unsigned getResY() const { return resolution.y; }
        glm::ivec2 getRes() const { return resolution; }

        virtual void setFrameLimit(unsigned fps)
        {
            framesPerSecond = fps;
            secondsPerFrame = 1.0 / double(fps);
            unlimited = false;
        }

        virtual void loop()
        {
            throttle();
        }
        
    protected:

        glm::ivec2 resolution;
        bool unlimited;
        unsigned framesPerSecond;
        double secondsPerFrame;
        std::chrono::high_resolution_clock::time_point lastFrame;

        virtual void throttle()
        {   
            if (unlimited)
            {
                return;
            }
            std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(t-lastFrame);
            if (elapsed.count() < secondsPerFrame)
            {
                double sleep = secondsPerFrame-elapsed.count();
                int64_t millis = int64_t(sleep*1000.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(millis));
            }
            lastFrame = std::chrono::high_resolution_clock::now();
        }

    };
}

#endif /* DISPLAY_H */
