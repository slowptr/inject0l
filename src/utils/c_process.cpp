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
    auto c_process::get_module_base(const std::string& module_name) const -> uint32_t {
        return reinterpret_cast<uint32_t>(get_module(module_name).modBaseAddr);
    }
    auto c_process::find_pattern(MODULEENTRY32 module, std::string_view pattern) -> uintptr_t {
        const auto module_base = reinterpret_cast<uintptr_t>(module.modBaseAddr);
        const auto module_size = module.modBaseSize;

        std::vector<uint8_t> module_bytes(module_size);

        const std::size_t page_size      = 4096;
        std::size_t       num_pages      = module_size / page_size;
        const std::size_t page_remainder = module_size % page_size;

        std::uintptr_t total_bytes_read = 0x0;
        auto           read_page = [&](const std::uintptr_t start, const std::size_t size) -> bool {
            DWORD old_protect;
            VirtualProtectEx(_handle, reinterpret_cast<void*>(start), size, PAGE_EXECUTE_READWRITE,
                             &old_protect);

            SIZE_T bytes_read;
            if (!ReadProcessMemory(_handle, reinterpret_cast<LPCVOID>(start),
                                   module_bytes.data() + total_bytes_read, size, &bytes_read))
                return false;

            total_bytes_read += bytes_read;
            VirtualProtectEx(_handle, reinterpret_cast<void*>(start), size, old_protect,
                             &old_protect);
            return bytes_read == size;
        };

        for (std::size_t i = 0; i < num_pages; i++) {
            if (!read_page(module_base + i * page_size, page_size)) return 0x0;
        }

        if (!read_page(module_base + --num_pages * page_size, page_remainder)) return 0x0;

        if (total_bytes_read != module_size) return 0x0;

        auto get_byte_vector_and_mask = [](const std::string&         pattern,
                                           std::vector<std::uint8_t>& byte_vec, std::string& mask) {
            for (std::size_t i = 0; i < pattern.length(); i++) {
                if (pattern[i] == ' ') continue;

                if (pattern[i] == '?') {
                    while (pattern[i + 1] == '?') i++;

                    mask.append("?");
                    byte_vec.push_back(0x0);
                    continue;
                }
                byte_vec.push_back(std::stoi(pattern.substr(i++, 2), nullptr, 16));
                mask.append("x");
            }
        };

        std::string               pattern_mask;
        std::vector<std::uint8_t> pattern_bytes;
        get_byte_vector_and_mask(pattern.data(), pattern_bytes, pattern_mask);

        std::uintptr_t pattern_offset = 0x0;
        for (std::size_t i = 0; i < module_size; i++) {
            if (module_bytes[i] == pattern_bytes[0]) {
                pattern_offset = i;

                for (std::size_t j = 1; j < pattern_bytes.size(); j++) {
                    const auto pattern_byte = pattern_bytes[j];
                    if (const auto mask_byte = pattern_mask[j]; mask_byte != '?') {
                        if (pattern_byte != module_bytes[i + j]) break;
                    }

                    if (j + 1 == pattern_bytes.size()) return module_base + pattern_offset;
                }
                pattern_offset = 0;
            }
        }
        return 0x0;
    }
    auto c_process::get_pid() const -> uint32_t { return _pid; }
    auto c_process::get_process_name() const -> std::string { return _process_name; }

    std::unique_ptr<c_process> g_process =
        std::make_unique<c_process>();  // Clang-Tidy: Initialization of 'g_process'
                                        // with static storage duration may throw an
                                        // exception that cannot be caught
}  // namespace utils