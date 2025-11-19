#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <unordered_map>
#include <chrono>

class Profiler
{
public:
    struct Record
    {
        double   total_time;
        uint32_t call_count;
    };

    class Scope
    {
    public:
        explicit Scope(const std::string& name);
        ~Scope();
    private:
        std::string m_name;
        std::chrono::high_resolution_clock::time_point m_start;
    };

    void add_record(const std::string& _name, double _time_ms);
    void new_frame();
    void end_frame();
    void print() const;

private:
    std::unordered_map<std::string, Record> m_records;
    std::chrono::high_resolution_clock::time_point m_frame_start;

    double m_frame_time = 0.0;
};

extern Profiler g_profiler;

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define PROFILE_SCOPE(name) Profiler::Scope CONCAT(_profiler_scope_, __LINE__)(name)

#endif
