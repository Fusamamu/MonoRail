#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include <SDL2/SDL.h>
#include <unordered_map>

class InputSystem
{
public:
    InputSystem();
    ~InputSystem();

    void update();

    // --- Keyboard ---
    bool is_key_down(SDL_Scancode sc) const;
    bool is_key_up  (SDL_Scancode sc) const;
    bool is_key_held(SDL_Scancode sc) const;

    // --- Mouse ---
    int get_mouse_x() const;
    int get_mouse_y() const;

    bool is_mouse_button_down(Uint8 button) const;

    int get_mouse_wheel_x() const;
    int get_mouse_wheel_y() const;

    // --- Quit ---
    bool get_quit_requested() const;

private:
    bool m_quit;

    std::unordered_map<SDL_Scancode, bool> m_key_down;
    std::unordered_map<SDL_Scancode, bool> m_key_up;
    std::unordered_map<SDL_Scancode, bool> m_key_held;

    std::unordered_map<Uint8, bool> m_mouse_buttons;
    int m_mouse_x = 0;
    int m_mouse_y = 0;

    int m_mouse_wheel_x = 0;
    int m_mouse_wheel_y = 0;
};

#endif // INPUTSYSTEM_HPP
