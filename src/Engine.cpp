#include "Engine.h"
#include "ApplicationConfig.h"

ApplicationConfig g_app_config;

Engine::Engine(): 
    m_is_running(true)
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
                g_app_config.SCREEN_WIDTH,
                g_app_config.SCREEN_HEIGHT,
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
        SDL_DestroyWindow   (p_window);
        SDL_Quit();
        return;
    }

    SDL_GL_MakeCurrent(p_window, context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();

    const char* glsl_version = "#version 150";
    ImGui_ImplSDL2_InitForOpenGL(p_window, context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ResourceManager::instance().init();

    MGUI::init();
    MGUI::ui_shader = ResourceManager::instance().get_shader("ui");
}

void Engine::update()
{
    glEnable(GL_DEPTH_TEST);

    Scene* _scene = scene_manager.create_scene("new scene", this);
    _scene->p_window = p_window;

    Material _phong_material;
    _phong_material.shader_id = "phong";

    Material _toon_material;
    _toon_material.shader_id = "toon";

    Material _fog_plane_material;
    _fog_plane_material.shader_id = "fog_plane";
    _fog_plane_material.depth_write = false;

    _scene->create_object("object", "teapot"      , {5.0f,  0.0f, 0.0f}, _phong_material);
    _scene->create_object("object", "monkey"      , {0.0f,  1.0f, 0.0f}, _toon_material) ;
    _scene->create_object("object", "arrow"       , {0.0f,  5.0f, 0.0f}, _toon_material) ;
    _scene->create_object("object", "large_plane" , {0.0f, -2.0f, 0.0f}, _fog_plane_material);

    entt::entity _agent_e = _scene->create_object("player", "teapot", {0.0f, 0.0f, 5.0f}, _toon_material);
    entt::entity _child_e = _scene->create_object("child" , "teapot", {0.0f, 0.0f, 8.0f}, _toon_material);

    _scene->get_registry().emplace<Agent>   (_agent_e);
    _scene->get_registry().emplace<Parent>  (_child_e).entity = _agent_e;

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


