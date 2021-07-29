#include "c_process.h"

#include <stdexcept>
#include <vector>

namespace utils {
    c_process::~c_process() {
        if (_handle) CloseHandle(_handle);
    }

    auto c_process::attach(const std::string& process_name) -> bool {
        if (process_name.empty())
            throw std::invalid_argument(
                "utils::c_process::attach(): "
                "process_name is empty.");

        _process_name = process_name;

        const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (!snapshot)
            throw std::runtime_error(
                "utils::c_process::attach(): "
                "couldn't create snapshot.");

        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        for (Process32First(snapshot, &entry); Process32Next(snapshot, &entry);) {
            if (process_name == entry.szExeFile) {
                _pid    = entry.th32ProcessID;
                _handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _pid);
                CloseHandle(snapshot);
                return true;
            }
        }
        return false;
    }
    auto c_process::get_module(const std::string& module_name) const -> MODULEENTRY32 {
        if (module_name.empty())
            throw std::invalid_argument(
                "utils::c_process::get_module_base(): "
                "module_name is empty.");

        const auto snapshot =
            CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _pid);

        if (!snapshot)
            throw std::runtime_error(
                "utils::c_process::get_module_base(): "
                "couldn't create snapshot.");

        MODULEENTRY32 entry;
        entry.dwSize = sizeof entry;

        for (Module32First(snapshot, &entry); Module32Next(snapshot, &entry);) {
            if (module_name == entry.szModule) {
                CloseHandle(snapshot);
                break;
            }
        }

        return entry;
    }
    auto c_process::get_pid() const -> uint32_t { return _pid; }
    auto c_process::get_process_name() const -> std::string { return _process_name; }

    std::unique_ptr<c_process> g_process =
        std::make_unique<c_process>();  // Clang-Tidy: Initialization of 'g_process'
                                        // with static storage duration may throw an
                                        // exception that cannot be caught
}  // namespace utils