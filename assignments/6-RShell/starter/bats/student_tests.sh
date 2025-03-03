#!/usr/bin/env bats

# File: student_tests.sh
# 
# Unit tests for remote shell (dsh)

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Check pwd command" {
    run ./dsh <<EOF
pwd
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [ -n "$output" ] # Ensure some output is returned
}

@test "Check echo command" {
    run ./dsh <<EOF
echo Hello, World!
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == "Hello, World!"* ]]
}

@test "Check changing directory (cd)" {
    run ./dsh <<EOF
cd /
pwd
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == "/"* ]]
}

@test "Check exit command" {
    run ./dsh <<EOF
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Check if file is created on remote server" {
    run ./dsh <<EOF
touch /tmp/remote_test_file
ls /tmp
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == *"remote_test_file"* ]]
}

@test "Check multiple commands execution" {
    run ./dsh <<EOF
echo First Command
echo Second Command
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == *"First Command"* ]]
    [[ "$output" == *"Second Command"* ]]
}
