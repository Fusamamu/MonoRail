#ifndef MGUI_H
#define MGUI_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "../UI/UIRenderer.h"

namespace MGUI
{
    struct Vec2  { float x, y; };
    struct Color { float r, g, b, a; };

    struct Window
    {
        Vec2 pos;
        Vec2 size;
        Vec2 cursor; // for placing widgets
        bool dragging = false;
        Vec2 drag_offset;   // mouse offset from top-left when drag starts
    };

    struct WindowState
    {
        Vec2 pos;
        Vec2 size;
        bool dragging = false;
        Vec2 drag_offset;
    };

    extern std::unordered_map<std::string, WindowState> window_states;

    struct Input
    {
        float mouse_x = 0;
        float mouse_y = 0;
        bool mouse_down     = false;
        bool mouse_pressed  = false;
        bool mouse_released = false;
    };

    extern Input      input;
    extern UIRenderer ui_renderer;
    extern Shader*    ui_shader;

    // UI state
    extern int hot_item;
    extern int active_item;
    extern std::vector<Window> window_stack;


    void init();

    void begin_frame();
    void process_event(const SDL_Event& e);

    void begin_window(const std::string& name, Vec2 pos, Vec2 size);
    void end_window();

    bool button(int id, const std::string& label, Vec2 size);

    void draw_rect(Vec2 pos, Vec2 size, Color color);
    void draw_text(const std::string& text, Vec2 pos, Color color = {1,1,1,1});

    // Utility
    bool mouse_over(Vec2 pos, Vec2 size);
}

#endif
