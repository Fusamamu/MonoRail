#ifndef GRID_H
#define GRID_H

#include "PCH.h"

class Grid {
public:
    Grid() = default;

    Grid(size_t width, size_t height):
        m_width (width),
        m_height(height)
    {
        resize(width, height);
    }

    ~Grid() = default;

    void resize(size_t width, size_t height)
    {
        m_width  = width;
        m_height = height;
        m_data.assign(width * height, entt::null);
    }

    size_t get_width () const { return m_width; }
    size_t get_height() const { return m_height; }

    entt::entity& at(size_t x, size_t y) {
        check_bounds(x, y);
        return m_data[y * m_width + x];
    }

    const entt::entity& at(size_t x, size_t y) const {
        check_bounds(x, y);
        return m_data[y * m_width + x];
    }

    void print() const {
        for (size_t y = 0; y < m_height; ++y) {
            for (size_t x = 0; x < m_width; ++x) {
                std::cout << (at(x, y) != entt::null ? "X " : ". ");
            }
            std::cout << "\n";
        }
    }

private:
    size_t m_width {0};
    size_t m_height{0};
    std::vector<entt::entity> m_data;

    void check_bounds(size_t x, size_t y) const {
        if (x >= m_width || y >= m_height) {
            throw std::out_of_range("Grid index out of bounds");
        }
    }
};

#endif
