{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs.buildPackages; [ 
    llvmPackages.libcxxStdenv
    llvmPackages.libcxxClang
    llvmPackages.compiler-rt
    llvmPackages.clangUseLLVM
    llvmPackages.bintools
    llvmPackages.libunwind
    libsodium
    mimalloc];
}
