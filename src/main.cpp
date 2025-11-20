#include "Engine.h"
#include <filesystem>
#include <mach-o/dyld.h>

std::filesystem::path getExecutableDirectory()
{
    char buffer[1024];
    uint32_t size = sizeof(buffer);
    _NSGetExecutablePath(buffer, &size);
    return std::filesystem::canonical(buffer).parent_path();
}

int main()
{
    auto exeDir = getExecutableDirectory();
    std::filesystem::current_path(exeDir);
    std::cout << "Working directory = "<< std::filesystem::current_path() << "\n";

    MUG::Engine _engine;
    _engine.init();
    _engine.update();
    _engine.quit();

    return 0;
}
