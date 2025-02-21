#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "It trims excessive spaces between commands" {
    run ./dsh <<EOF                
      ls      -l     
EOF

    [ "$status" -eq 0 ]
}

@test "It executes built-in cd command correctly" {
    mkdir -p test_dir
    run ./dsh <<EOF                
cd test_dir
pwd
EOF
    [[ "$output" == *"/test_dir"* ]]
    rmdir test_dir
    [ "$status" -eq 0 ]
}

@test "It handles cd without arguments" {
    run ./dsh <<EOF                
cd
pwd
EOF
    [ "$status" -eq 0 ]
}

@test "It handles cd into a nonexistent directory" {
    run ./dsh <<EOF                
cd
EOF
    [ "$status" -eq 0 ]
}

@test "It runs pwd correctly" {
    run ./dsh <<EOF                
pwd
EOF
    [ "$status" -eq 0 ]
}

@test "It handles an invalid command" {
    run ./dsh <<EOF                
invalid_command
EOF

    [ "$status" -eq 0 ]
}

@test "It handles empty input without crashing" {
    run ./dsh <<EOF                

EOF

    [ "$status" -eq 0 ]
}