#include "Engine.h"

Engine::Engine(): 
    m_is_running(true),
    SCREEN_WIDTH(1600),
    SCREEN_HEIGHT(1200)
{

}

Engine::~Engine()
{

}

void Engine::init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4  );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1  );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK , SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS   , 1  );
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES   , 4  );
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE           , 24 );
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER         , 1  );
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE         , 8  ); 

    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
   
    p_window = SDL_CreateWindow(
                "Help princess",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                window_flags);


    if (!p_window) 
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    context = SDL_GL_CreateContext(p_window);

    if (!context) 
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(p_window);
        SDL_Quit();
        return;
    }

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) 
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(p_window);
        SDL_Quit();
        return;
    }

    ResourceManager::instance().init();
}

void Engine::update()
{
    glEnable(GL_DEPTH_TEST);

    Scene* _scene = scene_manager.create_scene("new scene", this);
    _scene->p_window = p_window;
    _scene->on_enter();

    while(m_is_running)
    {
        _scene->on_update(0.016f);
        _scene->on_render(0.016f);
    }
    _scene->on_exit();
}

void Engine::request_quit()
{
    m_is_running = false;
}

void Engine::quit()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(p_window);
    SDL_Quit();
}


