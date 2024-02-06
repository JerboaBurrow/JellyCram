#ifndef DesktopDisplay_H
#define DesktopDisplay_H

#ifndef ANDROID

#include <cstdlib>
#include <vector>
#include <map>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <jGL/Display/event.h>
#include <jGL/Display/display.h>

#include <jGL/common.h>
#include <stb_image.h>
#include <algorithm>

namespace jGL
{

    void defaultKeyEventCallback
    (
        GLFWwindow * window,
        int key,
        int scancode,
        int action,
        int mods
    );

    class DesktopDisplay : public Display
    {
    public:

        struct Config
        {
            Config()
            : VULKAN(false), COCOA_RETINA(false)
            {}
            
            Config(bool v, bool c)
            : VULKAN(v), COCOA_RETINA(c)
            {}

            bool VULKAN;
            bool COCOA_RETINA;
        };

        DesktopDisplay
        (
            glm::ivec2 res,
            const char * title,
            GLFWkeyfun keyCallback,
            GLFWmousebuttonfun mouseButtonCallback,
            GLFWscrollfun mouseScrollCallback,
            const Config conf
        );

        DesktopDisplay
        (
            glm::ivec2 res,
            const char * title,
            const Config conf
        );

        ~DesktopDisplay(){ glfwTerminate(); free(logo); }

        GLFWwindow * getWindow() const { return glfwWindow; }

        bool isOpen(){ if (glfwWindow != NULL) { return !glfwWindow ? false : true; } return false; }
        bool closing(){ return glfwWindowShouldClose(glfwWindow); }
        
        void open(){ 
            if (glfwWindow == NULL)
            { 
                // required for MacOS
                //   https://www.glfw.org/faq.html#41__how_do_i_create_an_opengl_30_context
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, windowConfig.COCOA_RETINA);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                
                glfwWindow = glfwCreateWindow(getResX(), getResY(),title,NULL,NULL); glfwSwapInterval(1); 
            } 
        }

        void close(){ if (glfwWindow != NULL) { glfwDestroyWindow(glfwWindow); glfwWindow = NULL; } }

        void setAsFocus(){ if (glfwWindow != NULL) { glfwMakeContextCurrent(glfwWindow); } }

        void keyCallback
        (
            GLFWwindow * window,
            int key,
            int scancode,
            int action,
            int mods
        );

        void mousePosition(double & x, double & y){ if (glfwWindow != NULL){ glfwGetCursorPos(glfwWindow,&x,&y); } }

        void setMousePosition(double x, double y)
        {
            if (glfwWindow != NULL)
            {
                glfwSetCursorPos(glfwWindow, x, y);
            }
        }

        int getKeyLastState(int key) { return glfwGetKey(glfwWindow, key); }

        void loop() 
        { 
            data.clear();
            handleEvents(); 
            if (glfwWindowShouldClose(glfwWindow)){ close(); }
            throttle();
            swap(); 
        }

        std::vector<Event> getEvents(int code) 
        {
            if (data.events.find(code) == data.events.cend())
            {
                return {Event()};
            }
            else
            {
                return data.events[code];
            }
        }

        bool keyHasEvent(int key, EventType action) 
        {
            if (data.events.find(key) == data.events.cend())
            {
                return false;
            }
            else
            {
                auto ts = getEventTypes(key);
                return std::find(ts.cbegin(), ts.cend(), action) != ts.cend(); 
            }
        }

        std::vector<EventType> getEventTypes(int code) 
        {
            std::vector<EventType> e;
            if (data.events.find(code) == data.events.cend())
            {
                return {EventType::NONE};
            }
            else
            {
                for (auto evt : data.events[code])
                {
                    e.push_back(evt.type);
                }
                return e;
            }
        }

        Event getEvent(int code)
        {
            if (data.events.find(code) == data.events.cend())
            {
                return Event();
            }
            else
            {
                return data.events[code][0];
            }
        }

        struct WindowData 
        {
            std::map<int, std::vector<Event>> events;
            double scrollX = 0.0;
            double scrollY = 0.0;
            bool scrolled = false;

            void clear()
            {
                scrollX = 0.0;
                scrollY = 0.0;
                scrolled = false;
                events.clear();
            }
        };

    private:

        const char * title;

        GLFWimage * logo;

        GLFWwindow * glfwWindow;

        WindowData data;

        Config windowConfig;

        void swap(){ if (glfwWindow != NULL) { glfwSwapBuffers(glfwWindow); } }

        void handleEvents(){ if (glfwWindow != NULL){ glfwPollEvents(); } }

    };
}

#endif /* ANDROID */
#endif /* DesktopDisplay_H */