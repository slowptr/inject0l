#include <Windows.h>
#include "utils/c_log.h"
#include "c_injector.h"

int main(int argc, char** argv) {
    utils::g_log->set_console_handle(GetStdHandle(STD_OUTPUT_HANDLE));
    if (argc < 2) {
        utils::g_log->warn("usage: program.exe <process name> <dll name>");
        getchar();
        return 1;
    }

    try {
        c_injector injector{};
        injector.inject(argv[1], argv[2], true);
    } catch (const std::exception& e) {
        utils::g_log->warn(e.what());
    }

    return 0;
}