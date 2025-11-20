#include "Engine.h"
#include "ApplicationConfig.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Renderer/Material.h"
#include "Renderer/MeshRenderer.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include "Core/Time.h"
#include "Core/Profiler.h"

#include "Procedural/PerlinNoise.h"

ApplicationConfig g_app_config;

std::vector<glm::mat4> generateRandomInstances(int count)
{
    std::srand(static_cast<unsigned int>(std::time(0)));
    std::vector<glm::mat4> instanceModels;
    instanceModels.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        // Random position in a 50x50 area
        float x = static_cast<float>(std::rand() % 5000) / 100.0f - 25.0f;
        float z = static_cast<float>(std::rand() % 5000) / 100.0f - 25.0f;
        float y = 0.0f; // ground level

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

        // Random rotation around Y axis
        float angle = static_cast<float>(std::rand() % 360);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
        //
        // // Random uniform scale
        float scale = 0.5f + static_cast<float>(std::rand() % 50) / 100.0f; // 0.5 - 1.0
        model = glm::scale(model, glm::vec3(scale));

        instanceModels.push_back(model);
    }

    return instanceModels;
}

// Generate instance transforms for grass
std::vector<glm::mat4> generateGrassInstances(int patchCount, int bladesPerPatch, int layers)
{
    std::vector<glm::mat4> instanceModels;
    for (int p = 0; p < patchCount; p++)
    {
        // Random patch position
        float patchX = ((rand() % 1000) / 1000.0f - 0.5f) * 50.0f;
        float patchZ = ((rand() % 1000) / 1000.0f - 0.5f) * 50.0f;

        for (int b = 0; b < bladesPerPatch; b++)
        {
            float offsetX = ((rand() % 1000) / 1000.0f - 0.5f);
            float offsetZ = ((rand() % 1000) / 1000.0f - 0.5f);

            for (int l = 0; l < layers; l++)
            {
                glm::mat4 model = glm::mat4(1.0f);

                // Position
                model = glm::translate(model, glm::vec3(patchX + offsetX, l * 0.1f, patchZ + offsetZ));

                // Random rotation
                float angle = ((rand() % 1000) / 1000.0f) * 360.0f;
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0,1,0));

                // Random scale
                float scale = 0.8f + ((rand() % 200) / 1000.0f); // 0.8 - 1.0
                model = glm::scale(model, glm::vec3(scale, 1.0f, scale));

                instanceModels.push_back(model);
            }
        }
    }
    return instanceModels;
}

void create_grass(Scene* _scene)
{
    Material _grass_material;
    _grass_material.shader_id = "instance";

    entt::entity _grass_e = _scene->create_object("grass" , "grass_blade" , {10.0f, 10.0f, 0.0f}  , _grass_material) ;
    auto& _grass_mesh_renderer = _scene->get_registry().get<MeshRenderer>(_grass_e);
    std::vector<glm::mat4> instanceModels = generateRandomInstances(40000);
    //std::vector<glm::mat4> instanceModels = generateGrassInstances(100, 5, 5 );
    _grass_mesh_renderer.use_instancing = true;
    _grass_mesh_renderer.instance_count = instanceModels.size();
    //_grass_mesh_renderer.set_instance_data(instanceModels);
}

void create_grass_shell(Scene* _scene)
{
    Procgen::PerlinNoise _perlin_noise;
    GLuint _noise_id = _perlin_noise.generate_perlin_texture(512, 512, 100.0f, 1337);

    Material _shell_material;
    _shell_material.shader_id = "shell";
    _shell_material.diffuseMap = _noise_id;

    entt::entity _shell_e      = _scene->create_object("shell" , "large_plane" , { 0.0f,  1.0f, 0.0f}  , _shell_material) ;
    auto& _shell_transform     = _scene->get_registry().get<Component::Transform>   (_shell_e);
    auto& _shell_mesh_renderer = _scene->get_registry().get<MeshRenderer>(_shell_e);
    _shell_transform.scale = glm::vec3(0.3f, 0.3f, 0.3f);
    _shell_mesh_renderer.use_instancing = true;
    _shell_mesh_renderer.instance_count = 20;
}

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

        //_scene->create_object("object", "large_plane" , {0.0f, -2.0f, 0.0f}, _fog_plane_material);

        // _scene->create_object("object", "teapot"      , {5.0f,  0.0f, 0.0f}, _phong_material);
        // _scene->create_object("object", "monkey"      , {0.0f,  1.0f, 0.0f}, _toon_material) ;

        // entt::entity _agent_e = _scene->create_object("player", "teapot"      , { 0.0f,  0.0f, 5.0f}  , _toon_material);
        // entt::entity _child_e = _scene->create_object("child" , "teapot"      , { 0.0f,  0.0f, 8.0f}  , _toon_material);
        // _scene->get_registry().emplace<Agent>   (_agent_e);
        // _scene->get_registry().emplace<Parent>  (_child_e).entity = _agent_e;

        // create_grass      (_scene);
        // create_grass_shell(_scene);

        // auto _train_e = _scene->create_object("train", "train", { 0.0f, 0.5f, 0.0f }, _phong_material);
        // auto& _train_agent = _scene->get_registry().emplace<NAV::Agent>(_train_e);

        //_scene->create_object("floor", "large_plane" , {0.0f, 0.52f, 0.0f}, _phong_material);

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


