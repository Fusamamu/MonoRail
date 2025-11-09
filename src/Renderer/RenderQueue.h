#ifndef RENDERQUEUE_H
#define RENDERQUEUE_H

#include "../PCH.h"
#include "../Asset/AssetManager.h"
#include "Shader.h"
#include "RenderCommand.h"

class RenderQueue
{
public:
    RenderQueue() = default;
    ~RenderQueue() = default;

    void add    (const RenderCommand& _command);
    void sort   ();
    void execute();
    void clear  ();

private:
    std::vector<RenderCommand> m_render_commands;
};

#endif
