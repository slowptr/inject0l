#include <chrono>
#include <thread>
#include "utils/c_process.h"
#include "utils/c_log.h"

int main() {
    utils::g_log->set_console_handle(GetStdHandle(STD_OUTPUT_HANDLE));

    try {
        while (!utils::g_process->attach("example.exe")) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        utils::g_log->info("process " + utils::g_process->get_process_name() + " found.");

    } catch (const std::exception& e) {
        utils::g_log->warn(e.what());
    }

    return 0;
}