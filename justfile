buildDir := justfile_directory() / "build"
conanProfile := justfile_directory() / "conan_profile"

conan_install:
    conan install . --output-folder {{ buildDir }} --profile:build={{ conanProfile }} --profile:host={{ conanProfile }} --build=missing

build: conan_install
    cmake --preset conan-release
    cmake --build {{ buildDir }}

run: build
    {{ buildDir }}/vertex-cover

run_with_sample_input:
    #!/usr/bin/env bash
    just run <<EOF
    c This file describes a graph.
    p td 6 4
    1 2
    2 3
    c this is a comment and will be ignored 
    4 5
    4 6
    EOF
