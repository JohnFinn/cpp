{
  inputs = {nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";};
  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
    nativeBuildInputs = with pkgs; [
      conan
      cmake
      ninja
      flamegraph
      libunwind # to make clang work
      llvmPackages.clang-tools # https://github.com/NixOS/nixpkgs/issues/273875#issuecomment-2372118124
    ];
  in {
    devShells.${system}.default = pkgs.mkShell.override {stdenv = pkgs.clangStdenv;} {
      inherit nativeBuildInputs;
    };
  };
}
