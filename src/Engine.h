#ifndef ENGINE_H 
#define ENGINE_H

/**
 * @file Engine.h
 * @brief Declares the MUG::Engine class which manages windowing, rendering context,
 *        events, and scene management for the application.
 */

class SceneManager;

namespace MUG
{
    /**
     * @class Engine
     * @brief Core engine class responsible for initializing SDL, handling the main loop,
     *        routing events, and managing scene changes.
     */
    class Engine
    {
    public:
        SDL_Window*   p_window;
        SDL_GLContext context;
        SDL_Event     sdl_event;

        SceneManager* p_scene_manager;

        Engine();
        ~Engine();

        void init  ();
        void update();
        void quit  ();
        void request_quit();
    private:
        bool m_is_running;
    };
}

#endif
