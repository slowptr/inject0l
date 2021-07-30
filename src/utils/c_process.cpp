#include "c_process.h"

#include <stdexcept>
#include <TlHelp32.h>

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
    auto c_process::alloc_mem(const SIZE_T size) const -> void* {
        return VirtualAllocEx(_handle, nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }
    auto c_process::free_mem(void* memory) const -> void { VirtualFreeEx(_handle, memory, 0, MEM_RELEASE); }
    auto c_process::create_remote_thread(const LPTHREAD_START_ROUTINE address, void* memory) const -> void* {
        return CreateRemoteThread(_handle, nullptr, 0, address, memory, 0, 0);
    }
    auto c_process::get_process_name() const -> std::string { return _process_name; }
}  // namespace utils