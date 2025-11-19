#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <istream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <functional>
#include <optional>
#include <limits>
#include <numeric>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/transform.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#endif
