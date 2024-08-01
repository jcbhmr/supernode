#include <utility>
#include <filesystem>
#include <stdexcept>
#include <cosmo.h>
#include "../binlib.h"

int main(int argc, char *argv[]) {
    const auto app_cache_dir = binlib::cache_supernode();
    if (IsLinux() || IsXnu()) {
        binlib::exec_replace(app_cache_dir / "bin/npm", argv);
    } else if (IsWindows()) {
        const auto [bin, argv] = windows_script_argv(app_cache_dir / "npm.cmd", argc, argv);
        binlib::exec_replace(bin, argv);
    } else {
        throw std::runtime_error("unsupported os");
    }
}
