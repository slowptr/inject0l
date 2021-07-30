#ifndef INJECT0L_C_INJECTOR_H
#define INJECT0L_C_INJECTOR_H

#include "utils/c_process.h"
#include <string>

class c_injector {
   public:
    auto inject(const std::string& process_name, const std::string& dll_path, bool wait_for_process) -> bool;

   private:
    utils::c_process _process;
};

#endif  // INJECT0L_C_INJECTOR_H
