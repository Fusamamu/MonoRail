#include "RenderQueue.h"
#include "MeshRenderer.h"
#include "Renderer/Shader.h"
#include "Asset/AssetManager.h"

void RenderQueue::add(const RenderCommand& _command)
{
    m_render_commands.push_back(_command);
}

void RenderQueue::sort()
{
    std::sort(m_render_commands.begin(), m_render_commands.end(),
        [](const RenderCommand& _command_a, const RenderCommand& _command_b)
        {
            return _command_a.material < _command_b.material;
        });
}

void RenderQueue::execute()
{
    Shader* _selected_shader = nullptr;
    std::string _current_shader_id;

    for (const RenderCommand& _command : m_render_commands)
    {
        if (_command.material->shader_id != _current_shader_id)
        {
            _current_shader_id = _command.material->shader_id;

            _selected_shader = AssetManager::instance().get_shader(_current_shader_id);
            _selected_shader->use();
        }

        _selected_shader->set_mat4_uniform_model(_command.model_mat);

        if (_command.material->cast_shadow)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _command.shader_map);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _command.texture);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_3D, _command.ao_map);
        }

        _command.mesh_renderer->draw();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, 0);
    }
}

void RenderQueue::clear()
{
    m_render_commands.clear();
}

