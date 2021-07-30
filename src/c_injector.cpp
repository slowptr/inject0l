#include "c_injector.h"
#include "utils/c_log.h"
#include <thread>

auto c_injector::inject(const std::string& process_name, const std::string& dll_path, bool wait_for_process)
    -> bool {
    if (process_name.empty() || dll_path.empty())
        throw std::invalid_argument(
            "c_injector::inject(): "
            "process_name or dll_path is empty.");

    char dll[MAX_PATH];
    GetFullPathNameA(dll_path.c_str(), MAX_PATH, dll, nullptr);

    utils::g_log->info("c_injector::inject(): got dll path.");

    if (wait_for_process) {
        while (!_process.attach(process_name)) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } else
        _process.attach(process_name);

    utils::g_log->info("c_injector::inject(): process attached.");

    if (_process.get_process_name().empty())
        throw std::runtime_error(
            "c_injector::inject(): "
            "couldn't attach to process.");

    auto loadlibrary = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
    if (!loadlibrary)
        throw std::runtime_error(
            "c_injector::inject(): "
            "couldn't get LoadLibraryA address.");

    auto allocated_mem = _process.alloc_mem(strlen(dll));
    if (!allocated_mem)
        throw std::runtime_error(
            "c_injector::inject(): "
            "couldn't allocate memory.");

    utils::g_log->info("c_injector::inject(): memory allocated.");

    if (!_process.write<decltype(dll)>(allocated_mem, dll))
        throw std::runtime_error(
            "c_injector::inject(): "
            "couldn't write dll name into allocated memory.");

    utils::g_log->info("c_injector::inject(): dll name written into memory.");

    if (!_process.create_remote_thread(reinterpret_cast<LPTHREAD_START_ROUTINE>(loadlibrary), allocated_mem))
        throw std::runtime_error(
            "c_injector::inject(): "
            "couldn't create remote loadlibrary thread.");

    utils::g_log->info("c_injector::inject(): loadlibrary executed.");

    _process.free_mem(allocated_mem);
    return true;
}