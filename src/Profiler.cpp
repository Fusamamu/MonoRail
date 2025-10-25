#include "Profiler.h"

Profiler g_profiler;

Profiler::Scope::Scope(const std::string& _name):
    m_name(_name),
    m_start(std::chrono::high_resolution_clock::now())
{

}

Profiler::Scope::~Scope()
{
    auto _end = std::chrono::high_resolution_clock::now();
    double _elapsed = std::chrono::duration<double, std::milli>(_end - m_start).count();
    g_profiler.add_record(m_name, _elapsed);
}

void Profiler::add_record(const std::string& _name, double _time_ms)
{
    auto& _record = m_records[_name];
    _record.total_time += _time_ms;
    _record.call_count++;
}

void Profiler::new_frame()
{
    m_records.clear();
    m_frame_start = std::chrono::high_resolution_clock::now();
}

void Profiler::end_frame()
{
    auto _now = std::chrono::high_resolution_clock::now();
    m_frame_time = std::chrono::duration<double, std::milli>(_now - m_frame_start).count();
}

void Profiler::print() const
{
    std::cout << "\n--- Frame Profile (" << std::fixed << std::setprecision(2) << m_frame_time << " ms) ---\n";

    for (auto& [name, rec] : m_records)
    {
        double avg = rec.total_time / rec.call_count;
        double percent = (m_frame_time > 0.0) ? (avg / m_frame_time) * 100.0 : 0.0;

        std::cout << std::setw(16) << name << ": "
                  << std::setw(8) << avg << " ms ("
                  << std::setw(5) << percent << "%)\n";
    }
}

