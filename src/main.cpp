#include <Windows.h>
#include "utils/c_log.h"
#include "c_injector.h"

int main() {
    utils::g_log->set_console_handle(GetStdHandle(STD_OUTPUT_HANDLE));

    try {
        c_injector injector{};
        injector.inject("moh_spearhead.exe","./internal_template.dll", true);
    } catch (const std::exception& e) {
        utils::g_log->warn(e.what());
    }

    return 0;
}