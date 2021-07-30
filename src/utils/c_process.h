#ifndef EXTERNAL_TEMPLATE_C_PROCESS_H
#define EXTERNAL_TEMPLATE_C_PROCESS_H

#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>
#include <memory>
#include <string>

namespace utils {
    class c_process {
       public:
        c_process() = default;
        ~c_process();

        auto               attach(const std::string &process_name) -> bool;
        [[nodiscard]] auto get_module(const std::string &module_name) const -> MODULEENTRY32;
        auto               alloc_mem(SIZE_T size) -> void *;
        auto               free_mem(void *memory) -> void;
        auto               create_remote_thread(LPTHREAD_START_ROUTINE address, void *memory) -> void *;
        template <typename T>
        auto write(void *address, T value) -> bool {
            SIZE_T bytes;
            WriteProcessMemory(_handle, address, &value, sizeof(T), &bytes);
            return bytes == sizeof(T);
        }
        template <typename T>
        auto write(const uintptr_t address, T value) -> bool {
            SIZE_T bytes;
            WriteProcessMemory(_handle, reinterpret_cast<LPVOID>(address), &value, sizeof(T), &bytes);
            return bytes == sizeof(T);
        }

        [[nodiscard]] auto get_pid() const -> uint32_t;
        [[nodiscard]] auto get_process_name() const -> std::string;

       private:
        uint32_t    _pid = 0;
        void       *_handle{};
        std::string _process_name;
    };
}  // namespace utils

#endif  // EXTERNAL_TEMPLATE_C_PROCESS_H
