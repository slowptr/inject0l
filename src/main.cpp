#include <Windows.h>
#include "utils/c_log.h"

int main() {
    utils::g_log->set_console_handle(GetStdHandle(STD_OUTPUT_HANDLE));

    try {

    } catch (const std::exception& e) {
        utils::g_log->warn(e.what());
    }

    return 0;
}