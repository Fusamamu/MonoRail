#include "SceneManager.h"
#include "Scene/Scene.h"

Scene* SceneManager::create_scene(const std::string& name, Engine* _engine)
{
    auto it = scenes.find(name);
    if (it != scenes.end())
        return it->second.get();

    auto scene = std::make_unique<Scene>(name, _engine);
    Scene* scene_ptr = scene.get();
    scenes[name] = std::move(scene);

    return scene_ptr;
}

void SceneManager::switch_scene(const std::string& name)
{
    auto it = scenes.find(name);
    if (it == scenes.end())
    {
        std::cerr << "Scene " << name << " not found!\n";
        return;
    }

    if (current_scene)
        current_scene->on_exit();

    current_scene = it->second.get();
    current_scene->on_enter();
}

void SceneManager::update_current_scene(float delta_time)
{
    if (current_scene)
        current_scene->on_update(delta_time);
}

Scene* SceneManager::get_current_scene()
{
    return current_scene;
}
