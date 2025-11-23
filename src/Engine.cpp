#include "Engine.h"
#include "ApplicationConfig.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Asset/AssetManager.h"

#include "Renderer/Material.h"
#include "Renderer/MeshRenderer.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include "Core/Time.h"
#include "Core/Profiler.h"

#include "Procedural/PerlinNoise.h"

ApplicationConfig g_app_config;

namespace MUG
{
    Engine::Engine():m_is_running(true)
    {
        p_scene_manager = new SceneManager();
    }

    Engine::~Engine()
    {
        delete p_scene_manager;
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
                    "Mono rail",
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

        AssetManager::instance().init();

        MGUI::init();
        MGUI::load_csv();
        MGUI::ui_shader         = AssetManager::instance().get_shader("ui");
        MGUI::ui_texture_shader = AssetManager::instance().get_shader("ui_noise_texture");
        MGUI::text_shader       = AssetManager::instance().get_shader("text");


        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL Version: "  << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        std::cout << "Renderer: "      << glGetString(GL_RENDERER) << std::endl;
        std::cout << "Vendor: "        << glGetString(GL_VENDOR) << std::endl;

        SDL_version compiled;
        SDL_VERSION(&compiled);
        std::cout << "SDL version: "
                  << int(compiled.major) << "."
                  << int(compiled.minor) << "."
                  << int(compiled.patch) << std::endl;
    }

    void Engine::update()
    {
        glEnable(GL_DEPTH_TEST);

        Scene* _scene = p_scene_manager->create_scene("new scene", this);
        _scene->p_window = p_window;

        Material _phong_material;
        _phong_material.shader_id    = "phong";
        _phong_material.diffuse_color = glm::vec3(1.0f, 1.0f, 1.0f);

        Material _toon_material;
        _toon_material.shader_id = "toon";

        Material _fog_plane_material;
        _fog_plane_material.shader_id = "fog_plane";
        _fog_plane_material.depth_write = false;

        Core::Time::init();

        _scene->on_enter();
        while(m_is_running)
        {
            g_profiler.new_frame();

            PROFILE_SCOPE("Frame");

            Core::Time::update();
            _scene->on_update    (0.016f);
            _scene->on_render    (0.016f);
            _scene->on_render_gui(0.016f);

            {
                PROFILE_SCOPE("Swap");
                SDL_GL_SwapWindow(p_window);
            }

            g_profiler.end_frame();
            g_profiler.print();
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
}


