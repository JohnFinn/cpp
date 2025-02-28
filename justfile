buildDir := justfile_directory() / "build"
conanProfile := justfile_directory() / "conan_profile"

conan_install:
    conan install . --output-folder {{ buildDir }} --profile:build={{ conanProfile }} --profile:host={{ conanProfile }} --build=missing

build: conan_install
    cmake --preset conan-release
    cmake --build {{ buildDir }}

run: build
    {{ buildDir }}/vertex-cover
