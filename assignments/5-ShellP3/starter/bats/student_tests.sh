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

@test "Pipeline with Three Commands" {
    run "./dsh" <<EOF
ls | grep .c | wc -l
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="<expected_count>dsh3>dsh3>cmdloopreturned0"

    # Replace <expected_count> with the actual number of .c files
    count=$(ls | grep .c | wc -l | tr -d '[:space:]')
    expected_output="${count}dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Empty Input" {
    run "./dsh" <<EOF

EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Pipeline with echo and grep" {
    run "./dsh" <<EOF
echo "hello world" | grep hello
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="helloworlddsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Pipeline with cat and head" {
    run "./dsh" <<EOF
cat dshlib.c | head -n 5
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="<first_5_lines>dsh3>dsh3>cmdloopreturned0"

    # Replace <first_5_lines> with the first 5 lines of dshlib.c
    first_5_lines=$(head -n 5 dshlib.c | tr -d '[:space:]')
    expected_output="${first_5_lines}dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Pipeline with ls and sort" {
    run "./dsh" <<EOF
ls | sort
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="<sorted_ls_output>dsh3>dsh3>cmdloopreturned0"

    # Replace <sorted_ls_output> with the sorted output of `ls`
    sorted_ls_output=$(ls | sort | tr -d '[:space:]')
    expected_output="${sorted_ls_output}dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}