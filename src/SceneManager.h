#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "PCH.h"
#include "Scene.h"

class Engine;

class SceneManager {
public:
    SceneManager() = default;
    ~SceneManager() = default;

    Scene* create_scene(const std::string& name, Engine* _engine);
    void switch_scene(const std::string& name);
    void update_current_scene(float delta_time);
    Scene* get_current_scene();

private:
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* current_scene = nullptr;
};

#endif
