#include "../lib.h"
#include <cosmo.h>

int main(int argc, char *argv[]) {
    return supernode::internal::main(argc, argv, IsWindows() ? "node.exe" : "bin/node");
}
