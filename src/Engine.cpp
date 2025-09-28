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
}

void Engine::update()
{
    Camera _camera;

    _camera.position = { 10.0f, 10.0f, 10.0f };

    Shader _default_shader ("../res/shaders/toon.glsl");

    glm::mat4 _model = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f });
    glm::mat4 _view  = _camera.get_view_matrix();
    glm::mat4 _proj  = _camera.get_projection_matrix();

    _default_shader.use();
    _default_shader.set_mat4_uniform_model     (_model);
    _default_shader.set_mat4_uniform_view      (_view);
    _default_shader.set_mat4_uniform_projection(_proj);

    Sphere _cube;
    Mesh _quad_mesh = _cube.to_mesh();
    MeshRenderer _mesh_renderer;

    ResourceManager _resource_manager;
    Mesh _teapot = _resource_manager.load_model("../res/models/teapot.fbx");

    _mesh_renderer.load_mesh      (&_teapot);
    _mesh_renderer.set_buffer_data(&_teapot);

    InputSystem _input_system;

    glEnable(GL_DEPTH_TEST);

    while(m_is_running)
    {
        _input_system.update();

        if (_input_system.get_quit_requested())
        {
            m_is_running = false;
            return;
        }

        if (_input_system.is_key_held(SDL_SCANCODE_A))
        {
            _camera.camera_move_left(0.25f);
        }
        if (_input_system.is_key_held(SDL_SCANCODE_D))
        {
            _camera.camera_move_right(0.25f);
        }
        if (_input_system.is_key_held(SDL_SCANCODE_W))
        {
            _camera.camera_move_up(0.25f);
        }

        if (_input_system.is_key_held(SDL_SCANCODE_S))
        {
            _camera.camera_move_down(0.25f);
        }

        glm::mat4 _view  = _camera.get_view_matrix();
        _default_shader.use();
        _default_shader.set_mat4_uniform_view      (_view);
        
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _default_shader.use();
        _mesh_renderer.draw();

        SDL_GL_SwapWindow(p_window);
        SDL_Delay(16);
    }
}

void Engine::quit()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(p_window);
    SDL_Quit();
}


