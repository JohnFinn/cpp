{
  description = "A very basic flake";

  inputs = {nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";};

  outputs = {
    self,
    nixpkgs,
    ...
  } @ inputs: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    devShells.${system}.default = pkgs.mkShell {
      nativeBuildInputs = with pkgs; [conan cmake ninja];
    };

    packages.${system} = {
      default = pkgs.stdenv.mkDerivation {
        name = "default";
        src = ./conan_profile;

        buildPhase = ''
          echo default > $out
        '';
        phases = ["buildPhase"];
      };
      foo = pkgs.stdenv.mkDerivation {
        name = "foobar";
        src = ./conan_profile;

        buildPhase = ''
          echo foobar > $out
        '';
        phases = ["buildPhase"];
      };
    };

    # conan install . --output-folder ./build --profile:build=./conan_profile --profile:host=./conan_profile --build=missing
    # cmake --preset conan-release
    # cmake --build build/
  };
}
