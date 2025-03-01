buildDir := justfile_directory() / "build"
conanProfile := justfile_directory() / "conan_profile"

conan_install:
    conan install . --output-folder {{ buildDir }} --profile:build={{ conanProfile }} --profile:host={{ conanProfile }} --build=missing >&2

build: conan_install
    cmake --preset conan-release >&2
    cmake --build {{ buildDir }} >&2
    ln -f -s {{ buildDir }}/compile_commands.json {{ justfile_directory() }}/compile_commands.json >&2

run: build
    {{ buildDir }}/vertex-cover

runNoBuild:
    {{ buildDir }}/vertex-cover

gtest: build
    {{ buildDir }}/test

sample_input:
    #!/usr/bin/bash
    cat <<EOF
    c This file describes a graph.
    p td 6 4
    1 2
    2 3
    c this is a comment and will be ignored 
    4 5
    4 6
    EOF

run_with_sample_input:
    #!/usr/bin/env bash
    just sample_input | just run 

solve_and_check arg:
    /home/sunnari/code/algorithm-engineering/verifier.py {{ arg }} <(cat {{ arg }} | just runNoBuild)

smoke_test: build
    just solve_and_check <(just sample_input)
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_01_c_0.5_cost_matrix_component_nr_3128_size_6_cutoff_10.0.graph
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_01_c_0.5_cost_matrix_component_nr_3675_size_4_cutoff_10.0.graph
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_01_c_0.33_cost_matrix_component_nr_1170_size_8_cutoff_10.0.graph
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_01_c_0.33_cost_matrix_component_nr_3394_size_6_cutoff_10.0.graph
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_01_c_0.66_cost_matrix_component_nr_1214_size_6_cutoff_10.0.graph
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_01_c_0.66_cost_matrix_component_nr_3225_size_6_cutoff_10.0.graph
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_02_c_0.5_cost_matrix_component_nr_727_size_9_cutoff_10.0.graph
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_02_c_0.5_cost_matrix_component_nr_3046_size_11_cutoff_10.0.graph
    just solve_and_check /home/sunnari/code/algorithm-engineering/instances/biology/optK_02_c_0.33_cost_matrix_component_nr_1676_size_8_cutoff_10.0.graph
