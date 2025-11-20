#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>
#include <memory>
#include <unordered_map>

namespace MUG
{
    class Engine;
}

class Scene;

class SceneManager {
public:
    SceneManager() = default;
    ~SceneManager() = default;

    Scene* get_current_scene();
    Scene* create_scene(const std::string& name, MUG::Engine* _engine);
    void switch_scene(const std::string& name);
    void update_current_scene(float delta_time);

private:
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* current_scene = nullptr;
};

#endif
