#include "InputSystem.h"
#include "imgui_impl_sdl2.h"

InputSystem::InputSystem()
    : m_quit(false)
{
}

InputSystem::~InputSystem() = default;

void InputSystem::update()
{
    previous_mouse_pos = current_mouse_pos;
    int x, y;
    Uint32 buttons    = SDL_GetMouseState(&x, &y);
    current_mouse_pos = glm::vec2(float(x), float(y));

    // Clear per-frame states
    m_key_down.clear();
    m_key_up  .clear();

    m_mouse_down.clear();
    m_mouse_up  .clear();

    m_mouse_wheel_x = 0;
    m_mouse_wheel_y = 0;

    SDL_Event _event;
    while (SDL_PollEvent(&_event))
    {
        ImGui_ImplSDL2_ProcessEvent(&_event);
        MGUI::process_event(_event);

        switch (_event.type)
        {
            case SDL_QUIT:
                m_quit = true;
                break;

            // --- Keyboard ---
            case SDL_KEYDOWN:
                if (!_event.key.repeat)
                    m_key_down[_event.key.keysym.scancode] = true;
                m_key_held[_event.key.keysym.scancode] = true;

                if (_event.key.keysym.sym == SDLK_ESCAPE)
                    m_quit = true;
                break;

            case SDL_KEYUP:
                m_key_up[_event.key.keysym.scancode] = true;
                m_key_held[_event.key.keysym.scancode] = false;
                break;

            // --- Mouse motion ---
            case SDL_MOUSEMOTION:
                m_mouse_x = _event.motion.x;
                m_mouse_y = _event.motion.y;
                break;

            // --- Mouse buttons ---
            case SDL_MOUSEBUTTONDOWN:
                if (!m_mouse_held[_event.button.button]) // only trigger once
                    m_mouse_down[_event.button.button] = true;
                m_mouse_held[_event.button.button] = true;
                break;

            case SDL_MOUSEBUTTONUP:
                m_mouse_up  [_event.button.button] = true;
                m_mouse_held[_event.button.button] = false;
                break;

            // --- Mouse wheel ---
            case SDL_MOUSEWHEEL:
                m_mouse_wheel_x = _event.wheel.x;
                m_mouse_wheel_y = _event.wheel.y;
                break;
        }
    }
}

// --- Keyboard ---
bool InputSystem::is_key_down(SDL_Scancode sc) const
{
    auto it = m_key_down.find(sc);
    return it != m_key_down.end() && it->second;
}

bool InputSystem::is_key_up(SDL_Scancode sc) const
{
    auto it = m_key_up.find(sc);
    return it != m_key_up.end() && it->second;
}

bool InputSystem::is_key_held(SDL_Scancode sc) const
{
    auto it = m_key_held.find(sc);
    return it != m_key_held.end() && it->second;
}

// --- Mouse ---
int InputSystem::get_mouse_x() const { return m_mouse_x; }
int InputSystem::get_mouse_y() const { return m_mouse_y; }

glm::vec2 InputSystem::get_mouse_pos() const
{
    return glm::vec2(m_mouse_x, m_mouse_y);
}

bool InputSystem::is_mouse_down(Uint8 button) const
{
    auto it = m_mouse_down.find(button);
    return it != m_mouse_down.end() && it->second;
}

bool InputSystem::is_mouse_up(Uint8 button) const
{
    auto it = m_mouse_up.find(button);
    return it != m_mouse_up.end() && it->second;
}

bool InputSystem::is_mouse_button_held(Uint8 button) const
{
    auto it = m_mouse_held.find(button);
    return it != m_mouse_held.end() && it->second;
}

bool InputSystem::left_mouse_pressed() const
{
    return is_mouse_down(SDL_BUTTON_LEFT);
}

bool InputSystem::right_mouse_pressed() const
{
    return is_mouse_down(SDL_BUTTON_RIGHT);
}

bool InputSystem::left_mouse_held() const
{
    return is_mouse_button_held(SDL_BUTTON_LEFT);
}

bool InputSystem::right_mouse_held() const
{
    return is_mouse_button_held(SDL_BUTTON_RIGHT);
}

int InputSystem::get_mouse_wheel_x() const { return m_mouse_wheel_x; }
int InputSystem::get_mouse_wheel_y() const { return m_mouse_wheel_y; }

// --- Quit ---
bool InputSystem::get_quit_requested() const { return m_quit; }
