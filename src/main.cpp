#include <fmt/format.h>
#include <incbin.h>

INCBIN(NodeDarwinArm64Zip, "node-darwin-arm64.zip");
INCBIN(NodeDarwinX64Zip, "node-darwin-x64.zip");
INCBIN(NodeLinuxArm64Zip, "node-linux-arm64.zip");
INCBIN(NodeLinuxX64Zip, "node-linux-x64.zip");
INCBIN(NodeNodeModulesZip, "node-node_modules.zip");
INCBIN(NodeUnixLikeZip, "node-unixlike.zip");
INCBIN(NodeWinArm64Zip, "node-win-arm64.zip");
INCBIN(NodeWinX64Zip, "node-win-x64.zip");
INCBIN(NodeWinZip, "node-win.zip");

int main() {
    fmt::println("gNodeDarwinArm64ZipSize={}", gNodeDarwinArm64ZipSize);
    fmt::println("gNodeDarwinX64ZipSize={}", gNodeDarwinX64ZipSize);
    fmt::println("gNodeLinuxArm64ZipSize={}", gNodeLinuxArm64ZipSize);
    fmt::println("gNodeLinuxX64ZipSize={}", gNodeLinuxX64ZipSize);
    fmt::println("gNodeNodeModulesZipSize={}", gNodeNodeModulesZipSize);
    fmt::println("gNodeUnixLikeZipSize={}", gNodeUnixLikeZipSize);
    fmt::println("gNodeWinArm64ZipSize={}", gNodeWinArm64ZipSize);
    fmt::println("gNodeWinX64ZipSize={}", gNodeWinX64ZipSize);
    fmt::println("gNodeWinZipSize={}", gNodeWinZipSize);
    return 0;
}
