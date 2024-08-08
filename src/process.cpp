#include "process.h"
#include <fmt/format.h>

namespace supernode {
namespace process {

class command {
private:
    std::string m_program;
    std::string m_arg0;
    std::vector<std::string> m_args;

public:
    command(const std::string &program) noexcept : m_program(program), m_arg0(program) {}

    command& args(const std::vector<std::string> &args) noexcept {
        m_args = args;
        return *this;
    }

    command& arg0(const std::string &arg0) noexcept {
        m_arg0 = arg0;
        return *this;
    }

    std::exception exec() noexcept {
        auto program = m_program;
        auto argv = std::vector<const char *>();
        if (IsWindows() && (m_program.ends_with(".cmd") || m_program.ends_with(".bat"))) {
            if (const auto comspec = std::getenv("COMSPEC")) {
                argv.push_back(comspec);
            } else {
                argv.push_back("/C/Windows/System32/cmd.exe");
            }
            argv.push_back("/d");
            argv.push_back("/s");
            argv.push_back("/c");
            argv.push_back(m_program.c_str());
        } else {
            argv.push_back(m_arg0.c_str());
        }
        for (const auto &arg : m_args) {
            argv.push_back(arg.c_str());
        }
        argv.push_back(std::nullptr);

        if (IsWindows()) {
            // https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event
            struct sigaction sigint_handler;
            sigint_handler.sa_handler = [](int s) {};
            sigemptyset(&sigint_handler.sa_mask);
            sigint_handler.sa_flags = 0;
            sigaction(SIGINT, &sigint_handler, std::nullptr);

            pid_t pid;
            const auto err = posix_spawn(&pid, m_program.c_str(), std::nullptr, std::nullptr, argv.data(), std::nullptr);
            if (err) {
                return std::runtime_error(fmt::format("posix_spawn() {}: {}", m_program, strerror(err)));
            }
            int status;
            while (waitpid(pid, &status, 0) != -1)
            ;

            if (WIFEXITED(status)) {
                const auto exit_code = WEXITSTATUS(status);
                std::exit(exit_code);
            } else if (WIFSIGNALED(status)) {
                const auto signal = WTERMSIG(status);
                std::raise(signal);
                std::exit(128 + signal);
            } else if (WIFSTOPPED(status)) {
                const auto signal = WSTOPSIG(status);
                fmt::println("not implimented: WIFSTOPPED(status)=true, signal={}", signal);
                std::exit(100);
            } else if (WIFCONTINUED(status)) {
                fmt::println("not implimented: WIFCONTINUED(status)=true");
                std::exit(100);
            } else {
                std::unreachable();
            }
        } else {
            const auto err = execv(m_program.c_str(), argv.data());
            if (err) {
                return std::runtime_error(fmt::format("execv() {}: {}", m_program, strerror(err)));
            }
            std::unreachable();
        }
    }
}

} // namespace process
} // namespace supernode