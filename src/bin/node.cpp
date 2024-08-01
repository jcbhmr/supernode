#include <filesystem>
#include <stdexcept>
#include <cosmo.h>
#include "../binlib.h"

int main(int argc, char *argv[]) {
    const auto app_cache_dir = binlib::cache_supernode();
    if (IsLinux() || IsXnu()) {
        binlib::exec_replace(app_cache_dir / "bin/node", argv);
    } else if (IsWindows()) {
        binlib::exec_replace(app_cache_dir / "node.exe", argv);
    } else {
        throw std::runtime_error("unsupported os");
    }
}
