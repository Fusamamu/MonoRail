#include "../UI/UI.h"

namespace MGUI
{
    Input      input;
    UIRenderer ui_renderer;
    Shader*    ui_shader;

    std::unordered_map<std::string, WindowState> window_states;

    int hot_item = -1;
    int active_item = -1;
    std::vector<Window> window_stack;

    void init()
    {
        ui_renderer.init();
    }

    void begin_frame()
    {
        hot_item = -1;
        input.mouse_pressed  = false;
        input.mouse_released = false;
    }

    void process_event(const SDL_Event& e)
    {
        switch (e.type)
        {
            case SDL_MOUSEMOTION:
                input.mouse_x = static_cast<float>(e.motion.x);
                input.mouse_y = static_cast<float>(e.motion.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    input.mouse_down = true;
                    input.mouse_pressed = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    input.mouse_down = false;
                    input.mouse_released = true;
                }
                break;
            default:
                break;
        }
    }

    // ---------------- Window
    void begin_window(const std::string& name, MGUI::Vec2 pos, MGUI::Vec2 size)
    {
        // Get or create state
        WindowState& state = window_states[name];
        state.pos = state.pos.x != 0 || state.pos.y != 0 ? state.pos : pos;
        state.size = size;

        bool mouse_inside = mouse_over(state.pos, state.size);

        if (mouse_inside && input.mouse_pressed)
        {
            if (!state.dragging)
            {
                state.dragging = true;
                state.drag_offset.x = input.mouse_x - state.pos.x;
                state.drag_offset.y = input.mouse_y - state.pos.y;
            }
        }

        if (state.dragging)
        {
            if (input.mouse_down)
            {
                state.pos.x = input.mouse_x - state.drag_offset.x;
                state.pos.y = input.mouse_y - state.drag_offset.y;
            }
            else
                state.dragging = false;
        }

        // Layout cursor for widgets
        Window w;
        w.pos    = state.pos;
        w.size   = state.size;
        w.cursor = {10, 25};

        window_stack.push_back(w);

        // Draw window
        draw_rect(w.pos, w.size, {0.2f,0.2f,0.2f,1.0f});
        draw_text(name, {w.pos.x + 5, w.pos.y + 5});
    }


    void end_window()
    {
        if (!window_stack.empty())
            window_stack.pop_back();
    }

    // ---------------- Widgets
    bool button(int id, const std::string& label, Vec2 size)
    {
        if (window_stack.empty())
            return false;

        Window& w = window_stack.back();
        Vec2 pos = { w.pos.x + w.cursor.x, w.pos.y + w.cursor.y };

        bool clicked = false;

        if (mouse_over(pos, size))
            hot_item = id;

        if (active_item == -1 && hot_item == id && input.mouse_down)
            active_item = id;

        if (active_item == id && !input.mouse_down)
        {
            if (hot_item == id) clicked = true;
            active_item = -1;
        }

        // Draw button
        Color color = {0.7f, 0.7f, 0.7f, 1.0f};

        if (hot_item == id)
            color = {0.9f, 0.9f, 0.9f, 1.0f};
        if (active_item == id)
            color = {0.5f, 0.5f, 0.5f, 1.0f};

        draw_rect(pos, size, color);
        draw_text(label, {pos.x + 5, pos.y + 5});

        w.cursor.y += size.y + 5; // move cursor for next widget

        return clicked;
    }

    // ---------------- Drawing helpers
    void draw_rect(Vec2 pos, Vec2 size, Color color)
    {
        ui_shader->use();

        glm::mat4 _ui_model = glm::mat4(1.0f);
        _ui_model = translate(_ui_model, glm::vec3(pos.x , pos.y , 0.0f));
        _ui_model = scale    (_ui_model, glm::vec3(size.x, size.y, 1.0f));

        ui_shader->set_mat4_uniform_model(_ui_model);

        ui_renderer.draw();
    }

    void draw_text(const std::string& text, Vec2 pos, Color color)
    {
        // Implement text drawing using bitmap fonts or SDF
    }

    // ---------------- Utility
    bool mouse_over(Vec2 pos, Vec2 size)
    {
        return input.mouse_x >= pos.x && input.mouse_x <= pos.x + size.x &&
               input.mouse_y >= pos.y && input.mouse_y <= pos.y + size.y;
    }
}
