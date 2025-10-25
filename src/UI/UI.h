#ifndef MGUI_H
#define MGUI_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "../UI/UIRenderer.h"
#include "csv.hpp"
#include "stb_image.h"

namespace MGUI
{
    struct Texture
    {
        GLuint id;
        GLenum texture_target = GL_TEXTURE_2D;

        int width, height, nrComponents;
        unsigned char* p_data;

        void bind(int _slot = 0) const
        {
            glActiveTexture(GL_TEXTURE0 + _slot);
            glBindTexture(texture_target, id);
        }
    };

    struct Vec2  { float x, y; };
    struct Color { float r, g, b, a; };

    struct Glyph {
        float advance;
        float left_side_bearing;
        float planeMinX, planeMaxX, planeMaxY;
        float atlasX0, atlasY0, atlasX1, atlasY1;
        float u0, v0, u1, v1;
    };

    struct Atlas
    {
        std::map<int, Glyph> glyphs;
    };

    struct Character
    {
        unsigned int texture_id;
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;
    };

    class Text
    {
    public:
        float x, y;
        float scale;

        Text() = default;
        ~Text() = default;

        void set_text(const std::string& _text){ m_text = _text; }
        std::string get_text() { return m_text; }
    private:
        std::string m_text;
    };

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
    extern Shader*    ui_texture_shader;
    extern Shader*    text_shader;

    extern Texture atlas_texture;
    extern Atlas   atlas;

    // UI state
    extern int hot_item;
    extern int active_item;
    extern std::vector<Window> window_stack;

    void init();
    void load_csv();

    void begin_frame();
    void process_event(const SDL_Event& e);

    void begin(const std::string& name);

    void begin_window(const std::string& name, Vec2 pos, Vec2 size);
    void end_window();

    bool button(int id, const std::string& label, Vec2 size);

    void draw_rect(Vec2 pos, Vec2 size, Color color);
    void draw_text(const std::string& text, Vec2 pos, Color color = {1,1,1,1});
    void draw_texture(Vec2 _position, Vec2 _size, GLuint _texture_id);

    // Utility
    bool mouse_over(Vec2 pos, Vec2 size);
}

#endif
