#ifndef ENGINE_H 
#define ENGINE_H 

class SceneManager;

namespace MUG
{
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
