#include "binlib.h"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <optional>
#include <utility>
#include <vector>
#include <cosmo.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <fmt/format.h>

namespace binlib {
std::filesystem::path cache_supernode() {
    auto app_cache_dir = dirs::cache_dir().value() / (std::string(PROJECT_NAME) + "-" + std::string(PROJECT_VERSION));

    if (!std::filesystem::exists(app_cache_dir)) {
        std::filesystem::create_directories(app_cache_dir);
        try {
            if (IsLinux()) {
                #ifdef __x86_64__
                filesystemzip::copy("/zip/node-linux-x64", app_cache_dir, std::filesystem::copy_options::recursive);
                #elif defined(__aarch64__)
                filesystemzip::copy("/zip/node-linux-arm64", app_cache_dir, std::filesystem::copy_options::recursive);
                #else
                #error "unsupported architecture"
                #endif
            } else if (IsXnu()) {
                #ifdef __x86_64__
                filesystemzip::copy("/zip/node-darwin-x64", app_cache_dir, std::filesystem::copy_options::recursive);
                #elif defined(__aarch64__)
                filesystemzip::copy("/zip/node-darwin-arm64", app_cache_dir, std::filesystem::copy_options::recursive);
                #else
                #error "unsupported architecture"
                #endif
            } else if (IsWindows()) {
                #ifdef __x86_64__
                filesystemzip::copy("/zip/node-windows-x64", app_cache_dir, std::filesystem::copy_options::recursive);
                #elif defined(__aarch64__)
                filesystemzip::copy("/zip/node-windows-arm64", app_cache_dir, std::filesystem::copy_options::recursive);
                #else
                #error "unsupported architecture"
                #endif
            } else {
                throw std::runtime_error("unsupported os");
            }

            if (IsLinux() || IsXnu()) {
                filesystemzip::copy("/zip/node-unixlike", app_cache_dir, std::filesystem::copy_options::recursive);
            } else if (IsWindows()) {
                filesystemzip::copy("/zip/node-win", app_cache_dir, std::filesystem::copy_options::recursive);
            } else {
                throw std::runtime_error("unsupported os");
            }

            if (IsLinux() || IsXnu()) {
                filesystemzip::copy("/zip/node-node_modules", app_cache_dir / "lib/node_modules", std::filesystem::copy_options::recursive);
            } else if (IsWindows()) {
                filesystemzip::copy("/zip/node-node_modules", app_cache_dir / "node_modules", std::filesystem::copy_options::recursive);
            } else {
                throw std::runtime_error("unsupported os");
            }
        } catch (...) {
            std::filesystem::remove_all(app_cache_dir);
            throw;
        }
    }

    return app_cache_dir;
}

[[noreturn]] void exec_replace(const std::filesystem::path path, char *const argv[]) {
    if (IsWindows()) {
        // https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event
        struct sigaction sigint_handler;
        sigint_handler.sa_handler = [](int s) {};
        sigemptyset(&sigint_handler.sa_mask);
        sigint_handler.sa_flags = 0;
        sigaction(SIGINT, &sigint_handler, NULL);

        pid_t pid;
        const auto err = posix_spawn(&pid, path.c_str(), NULL, NULL, argv, NULL);
        if (err) {
          throw std::runtime_error(fmt::format("posix_spawn() {}: {} ({})", path.string(), err, strerror(err)));
        }
        pid_t status;
        while (waitpid(pid, &status, 0) != -1)
          ;
        std::exit(status);
    } else {
        const auto err = execv(path.c_str(), argv);
        if (err) {
            throw std::runtime_error(fmt::format("execv() {}: {} ({})", path.string(), err, strerror(err)));
        }
        std::unreachable();
    }
}

std::filesystem::path comspec() {
    if (!IsWindows()) {
        throw std::runtime_error("not windows");
    }

    if (const auto comspec = std::getenv("COMSPEC")) {
        return std::filesystem::path(comspec);
    } else {
        return std::filesystem::path("/C/Windows/System32/cmd.exe");
    }
}

std::pair<std::filesystem::path, char* const*> windows_script_argv(const std::filesystem::path &path, int argc, char *const argv[]) {
    if (!IsWindows()) {
        throw std::runtime_error("not windows");
    }

    const auto comspec = binlib::comspec();
    auto new_argv = std::vector<char *>();
    new_argv.push_back(comspec.c_str());
    new_argv.push_back("/d");
    new_argv.push_back("/s");
    new_argv.push_back("/c");
    new_argv.push_back(path.c_str());
    for (int i = 1; i < argc; i++) {
        new_argv.push_back(argv[i]);
    }
    new_argv.push_back(nullptr);
    return {comspec, new_argv.data()};
}

} // namespace binlib

namespace dirs {

std::optional<std::filesystem::path> home_dir() {
    if (const auto home = std::getenv("HOME")) {
        return std::filesystem::path(home);
    } else {
        return std::nullopt;
    }
}

std::optional<std::filesystem::path> state_dir() {
    if (IsLinux()) {
        if (const auto xdg_state_home = std::getenv("XDG_STATE_HOME")) {
            return std::filesystem::path(xdg_state_home);
        } else if (const auto home = dirs::home_dir()) {
            return *home / ".local/state";
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }
}

std::optional<std::filesystem::path> cache_dir() {
    if (IsLinux()) {
        if (const auto xdg_cache_home = std::getenv("XDG_CACHE_HOME")) {
            return std::filesystem::path(xdg_cache_home);
        } else if (const auto home = dirs::home_dir()) {
            return *home / ".cache";
        } else {
            return std::nullopt;
        }
    } else if (IsXnu()) {
        if (const auto home = dirs::home_dir()) {
            return *home / "Library/Caches";
        } else {
            return std::nullopt;
        }
    } else if (IsWindows()) {
        if (const auto localappdata = std::getenv("LOCALAPPDATA")) {
            return std::filesystem::path(localappdata);
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }
}

} // namespace dirs

namespace filesystemzip {

void copy(const std::filesystem::path &from, const std::filesystem::path &to) {
    filesystemzip::copy(from, to, std::filesystem::copy_options::none);
}

void copy(const std::filesystem::path &from, const std::filesystem::path &to, std::filesystem::copy_options options) {
    if (options != std::filesystem::copy_options::recursive) {
        throw std::runtime_error("options must be std::filesystem::copy_options::recursive");
    }
    
    std::filesystem::create_directories(to);
    for (const auto &entry : std::filesystem::directory_iterator(from)) {
        const auto to_path = to / entry.path().filename();
        if (entry.is_directory()) {
            std::filesystem::create_directories(to_path);
            filesystemzip::copy(entry, to_path, options);
        } else if (entry.is_regular_file()) {
            filesystemzip::copy_file(entry, to_path, std::filesystem::copy_options::none);
        } else {
            throw std::runtime_error(fmt::format("{} must be a directory or a regular file", entry.path().string()));
        }
    }
}

void copy_file(const std::filesystem::path &from, const std::filesystem::path &to) {
    filesystemzip::copy_file(from, to, std::filesystem::copy_options::none);
}

void copy_file(const std::filesystem::path &from, const std::filesystem::path &to, std::filesystem::copy_options options) {
    if (options != std::filesystem::copy_options::none) {
        throw std::runtime_error("options must be std::filesystem::copy_options::none");
    }
    
    auto ifs = std::ifstream(from, std::ios::binary);
    auto ofs = std::ofstream(to, std::ios::binary);
    ofs << ifs.rdbuf();
    ifs.close();
    ofs.close();
    std::filesystem::permissions(to, std::filesystem::status(from).permissions());
}

} // namespace filesystemzip
