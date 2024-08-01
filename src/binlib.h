#pragma once
#include <filesystem>
#include <optional>
#include <utility>

namespace dirs {
std::optional<std::filesystem::path> home_dir();
std::optional<std::filesystem::path> state_dir();
std::optional<std::filesystem::path> cache_dir();
} // namespace dirs

namespace filesystemzip {
void copy(const std::filesystem::path &from, const std::filesystem::path &to);
void copy(const std::filesystem::path &from, const std::filesystem::path &to, std::filesystem::copy_options options);
void copy_file(const std::filesystem::path &from, const std::filesystem::path &to);
void copy_file(const std::filesystem::path &from, const std::filesystem::path &to, std::filesystem::copy_options options);
} // namespace filesystemzip

namespace binlib {
std::filesystem::path cache_supernode();
[[noreturn]] void exec_replace(const std::filesystem::path, char *const argv[]);
std::filesystem::path comspec();
std::pair<std::filesystem::path, char**> windows_script_argv(const std::filesystem::path path, int argc, char *const argv[]);
} // namespace binlib