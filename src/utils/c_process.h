#ifndef EXTERNAL_TEMPLATE_C_PROCESS_H
#define EXTERNAL_TEMPLATE_C_PROCESS_H

#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>
#include <string>
#include <memory>

namespace utils {
    class c_process {
       public:
        c_process() = default;
        ~c_process();

        auto               attach(const std::string& process_name) -> bool;
        [[nodiscard]] auto get_module(const std::string& module_name) const -> MODULEENTRY32;
        [[nodiscard]] auto get_module_base(const std::string& module_name) const -> uint32_t;

        template <typename T>
        auto read(const uintptr_t address) -> T {
            T temp_val;
            ReadProcessMemory(_handle, reinterpret_cast<LPCVOID>(address), &temp_val, sizeof(T),
                              nullptr);
            return temp_val;
        }

        template <typename T>
        auto write(const uintptr_t address, T value) -> bool {
            SIZE_T bytes;
            WriteProcessMemory(_handle, reinterpret_cast<LPVOID>(address), &value, sizeof(T),
                               &bytes);
            return bytes == sizeof(T);
        }

        auto find_pattern(MODULEENTRY32 module, std::string_view pattern) -> uintptr_t;

        [[nodiscard]] auto get_pid() const -> uint32_t;
        [[nodiscard]] auto get_process_name() const -> std::string;

       private:
        uint32_t    _pid = 0;
        void*       _handle{};
        std::string _process_name;
    };
    extern std::unique_ptr<c_process> g_process;
}  // namespace utils

#endif  // EXTERNAL_TEMPLATE_C_PROCESS_H
