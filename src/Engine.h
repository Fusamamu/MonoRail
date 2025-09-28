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
#include "ResourceManager.h"
#include "InputSystem.h"

class Engine
{
public:
    unsigned int SCREEN_WIDTH;
    unsigned int SCREEN_HEIGHT;

    SDL_Window*   p_window;
    SDL_GLContext context;
    SDL_Event     sdl_event;

    Engine();
    ~Engine();
    
    void init  ();
    void update();
    void quit  ();
private:
    bool m_is_running;
};

#endif
