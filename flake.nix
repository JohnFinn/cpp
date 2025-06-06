{
  description = "A very basic flake";

  inputs = {nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";};

  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
    nativeBuildInputs = with pkgs; [conan cmake ninja gtest];
  in {
    devShells.${system}.default = pkgs.mkShell {
      inherit nativeBuildInputs;
    };

    packages.${system} = {
      default = pkgs.stdenv.mkDerivation {
        name = "foo";
        src = ./.;
        inherit nativeBuildInputs;

        buildPhase = ''
          cmake -S $src -B $out -G Ninja
          ninja -C $out
        '';
        phases = ["buildPhase"];
      };
    };

    # conan install . --output-folder ./build --profile:build=./conan_profile --profile:host=./conan_profile --build=missing
    # cmake --preset conan-release
    # cmake --build build/
  };
}
