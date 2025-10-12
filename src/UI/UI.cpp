#include "../UI/UI.h"

namespace MGUI
{
    Input      input;
    UIRenderer ui_renderer;
    Shader*    ui_shader;
    Shader*    text_shader;

    Texture atlas_texture;
    Atlas   atlas;

    std::unordered_map<std::string, WindowState> window_states;

    int hot_item    = -1;
    int active_item = -1;
    std::vector<Window> window_stack;


    GLuint VAO, VBO;

    void init()
    {
        ui_renderer.init();


        float quadVertices[6][4]; // 6 vertices per glyph quad: x,y,u,v
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), nullptr, GL_DYNAMIC_DRAW);

        // position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texcoords
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void load_csv()
    {
        atlas_texture.p_data = stbi_load("../res/fonts/H5H/H5H.png", &atlas_texture.width, &atlas_texture.height, &atlas_texture.nrComponents, 0);
        if(!atlas_texture.p_data)
            stbi_image_free(atlas_texture.p_data);

        assert(atlas_texture.p_data != nullptr && "No data loaded!");

        GLenum _format;

        switch(atlas_texture.nrComponents)
        {
            case 1: _format = GL_RED ; break;
            case 3: _format = GL_RGB ; break;
            case 4: _format = GL_RGBA; break;
            default:
                std::cerr << "Unsupported image format: " << atlas_texture.nrComponents << " channels\n";
            stbi_image_free(atlas_texture.p_data);
            assert(false && "Unsupported image format");
        }

        glGenTextures   (1, &atlas_texture.id);
        glBindTexture   (GL_TEXTURE_2D, atlas_texture.id);
        glTexImage2D    (GL_TEXTURE_2D, 0, _format, atlas_texture.width, atlas_texture.height, 0, _format, GL_UNSIGNED_BYTE, atlas_texture.p_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture   (GL_TEXTURE_2D, 0);

        stbi_image_free(atlas_texture.p_data);





        csv::CSVReader reader("../res/fonts/H5H/H5H.csv");

        float atlasWidth  = 444.0f;  // <-- set your atlas texture width here
        float atlasHeight = 444.0f;  // <-- set your atlas texture height here

        for (csv::CSVRow& row : reader)
        {
            int id = row[0].get<int>();

            float advance   = row[1].get<float>();
            float lsb       = row[2].get<float>();
            float planeMinX = row[3].get<float>();
            float planeMaxX = row[4].get<float>();
            float planeMaxY = row[5].get<float>();
            float atlasX0   = row[6].get<float>();
            float atlasY0   = row[7].get<float>();
            float atlasX1   = row[8].get<float>();
            float atlasY1   = row[9].get<float>();

            // --- Normalize to 0.0–1.0 UVs ---
            float u0 = atlasX0 / atlasWidth;
            float v0 = 1.0f - (atlasY1 / atlasHeight); // flip Y for OpenGL
            float u1 = atlasX1 / atlasWidth;
            float v1 = 1.0f - (atlasY0 / atlasHeight);

            Glyph g = {
                advance, lsb, planeMinX, planeMaxX, planeMaxY,
                atlasX0, atlasY0, atlasX1, atlasY1,
                u0, v0, u1, v1
            };

            atlas.glyphs[id] = g;

            std::cout << "Glyph " << id << ": "
                << "advance=" << advance
                << ", lsb=" << lsb
                << ", planeMinX=" << planeMinX
                << ", planeMaxX=" << planeMaxX
                << ", planeMaxY=" << planeMaxY
                << ", u0=" << u0
                << ", v0=" << v0
                << ", u1=" << u1
                << ", v1=" << v1 << std::endl;
        }

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

        //Draw window
        draw_rect(w.pos, w.size, {0.2f,0.2f,0.2f,1.0f});
        draw_text(name, {w.pos.x, w.pos.y });
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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas_texture.id);

        ui_shader->use();
        ui_shader->set_int("u_texture", 0);

        glm::mat4 _ui_model = glm::mat4(1.0f);
        _ui_model = translate(_ui_model, glm::vec3(pos.x , pos.y , 0.0f));
        _ui_model = scale    (_ui_model, glm::vec3(size.x, size.y, 1.0f));

        ui_shader->set_mat4_uniform_model(_ui_model);

        ui_renderer.draw();
    }

    void draw_text(const std::string& text, Vec2 pos, Color color)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas_texture.id);

        text_shader->use();
        text_shader->set_int("u_texture", 0);

        glm::mat4 _ui_model = glm::mat4(1.0f);
        _ui_model = translate(_ui_model, glm::vec3(pos.x , pos.y , 0.0f));
        text_shader->set_mat4_uniform_model(_ui_model);
        text_shader->set_vec4("u_color", glm::vec4(1.0, 1.0, 1.0, 1.0));

        glBindVertexArray(VAO);

        float _advance;

        for (char c : text)
        {
            if (atlas.glyphs.find(c) == atlas.glyphs.end())
                continue; // skip missing

            float scale = 100.0f;

            Glyph g = atlas.glyphs[c];

            // float xpos = 0.0f + _advance;
            // float ypos = 0.0f;

            float xpos = _advance + g.planeMinX * scale;
            float ypos = g.planeMinY * scale; // use planeMinY (relative to baseline)

            float w = (g.planeMaxX - g.planeMinX) * scale;
            float h = g.planeMaxY                 * scale;

            // texture coordinates
            float u0 = g.u0;
            float v0 = g.v0;
            float u1 = g.u1;
            float v1 = g.v1;

            std::cout << u0 << ", " << v0 << ", " << u1 << ", " << v1 << std::endl;

            float vertices[6][4] = {
                {xpos,     ypos + h, u0, v1},
                {xpos,     ypos,     u0, v0},
                {xpos + w, ypos,     u1, v0},

                {xpos,     ypos + h, u0, v1},
                {xpos + w, ypos,     u1, v0},
                {xpos + w, ypos + h, u1, v1}
            };

            // Update VBO
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            // Draw quad
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Advance cursor
            _advance += g.advance * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // ---------------- Utility
    bool mouse_over(Vec2 pos, Vec2 size)
    {
        return input.mouse_x >= pos.x && input.mouse_x <= pos.x + size.x &&
               input.mouse_y >= pos.y && input.mouse_y <= pos.y + size.y;
    }
}
