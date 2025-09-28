#include "InputSystem.h"

InputSystem::InputSystem()
    : m_quit(false)
{
}

InputSystem::~InputSystem() = default;

void InputSystem::update()
{
    m_key_down.clear();
    m_key_up.clear();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                m_quit = true;
                break;

            case SDL_KEYDOWN:
                if (!event.key.repeat)
                    m_key_down[event.key.keysym.scancode] = true;
                m_key_held[event.key.keysym.scancode] = true;

                if (event.key.keysym.sym == SDLK_ESCAPE)
                    m_quit = true;
                break;

            case SDL_KEYUP:
                m_key_up  [event.key.keysym.scancode] = true;
                m_key_held[event.key.keysym.scancode] = false;
                break;

            case SDL_MOUSEMOTION:
                m_mouse_x = event.motion.x;
                m_mouse_y = event.motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                m_mouse_buttons[event.button.button] = true;
                break;

            case SDL_MOUSEBUTTONUP:
                m_mouse_buttons[event.button.button] = false;
                break;

            case SDL_MOUSEWHEEL:
                m_mouse_wheel_x = event.wheel.x;
                m_mouse_wheel_y = event.wheel.y;
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

bool InputSystem::is_mouse_button_down(Uint8 button) const
{
    auto it = m_mouse_buttons.find(button);
    return it != m_mouse_buttons.end() && it->second;
}

int InputSystem::get_mouse_wheel_x() const { return m_mouse_wheel_x; }
int InputSystem::get_mouse_wheel_y() const { return m_mouse_wheel_y; }

// --- Quit ---
bool InputSystem::get_quit_requested() const { return m_quit; }
