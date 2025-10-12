#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include "../UI/UI.h"

namespace MGUI
{
    class TextRenderer
    {
    public:
        TextRenderer() = default;
        ~TextRenderer() = default;

        void set_buffer_data();
        void draw(Text& _text, std::map<GLchar, Character> _characters);
    private:
        unsigned int m_vao, m_vbo;
    };
}

#endif
