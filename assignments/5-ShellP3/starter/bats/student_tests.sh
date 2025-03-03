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

# Test if dsh can execute a simple command
@test "Basic command execution: ls" {
    run ./dsh <<EOF
ls
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh" ]] # Adjust based on expected files in your directory
}

# Test if the 'cd' command works properly
@test "Built-in command: cd" {
    run ./dsh <<EOF
cd ..
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/" ]] # Ensure it prints a valid path
}

# Test if 'exit' command properly terminates the shell
@test "Built-in command: exit" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

# Test if pipes work correctly
@test "Piped command: ls | grep student_tests.sh" {
    run ./dsh <<EOF
ls | grep student_tests.sh
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "student_tests.sh" ]]
}

# Test multiple pipes at a time
@test "Multiple pipes: ls | grep dsh | sort" {
    run ./dsh <<EOF
ls | grep dsh | sort
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh" ]]
}
