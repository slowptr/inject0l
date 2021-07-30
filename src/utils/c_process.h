#ifndef EXTERNAL_TEMPLATE_C_PROCESS_H
#define EXTERNAL_TEMPLATE_C_PROCESS_H

#include <Windows.h>

#include <cstdint>
#include <string>

namespace utils {
    class c_process {
       public:
        c_process() = default;
        ~c_process();

        auto attach(const std::string &process_name) -> bool;
        auto alloc_mem(SIZE_T size) const -> void *;
        auto free_mem(void *memory) const -> void;
        auto create_remote_thread(LPTHREAD_START_ROUTINE address, void *memory) const -> void *;
        template <typename T>
        auto write(void *address, T value) -> bool {
            SIZE_T bytes;
            WriteProcessMemory(_handle, address, value, sizeof(T), &bytes);
            return bytes == sizeof(T);
        }

        [[nodiscard]] auto get_process_name() const -> std::string;

       private:
        uint32_t    _pid = 0;
        void       *_handle{};
        std::string _process_name;
    };
}  // namespace utils

#endif  // EXTERNAL_TEMPLATE_C_PROCESS_H
