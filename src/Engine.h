#ifndef ENGINE_H 
#define ENGINE_H 

#include "PCH.h"
#include "Mesh.h"
#include "Sphere.h"
#include "Cube.h"
#include "Quad.h"
#include "MeshRenderer.h"
#include "Shader.h"
#include "Camera.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputSystem.h"
#include "Component.h"
#include "UI/UI.h"
#include "Time.h"

class Engine
{
public:
    SDL_Window*   p_window;
    SDL_GLContext context;
    SDL_Event     sdl_event;

    SceneManager scene_manager;

    Engine();
    ~Engine();
    
    void init  ();
    void update();
    void quit  ();
    void request_quit();
private:
    bool m_is_running;
};

#endif
