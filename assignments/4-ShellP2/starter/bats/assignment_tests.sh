#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

# Create a symbolic link to dsh if it doesn't exist
setup() {
    if [ ! -L "./dsh" ]; then
        ln -s /mnt/c/Users/amida/Desktop/4-ShellP2\ -\ Copy/starter/dsh ./dsh
        echo "Symlink created for dsh"
    else
        echo "Symlink for dsh already exists"
    fi
}

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Exit command should terminate the shell" {
    run "./dsh" <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Change directory - valid directory" {
    current=$(pwd)

    cd /tmp
    mkdir -p dsh-test

    run "${current}/dsh" <<EOF
cd /tmp/dsh-test
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    [[ "$stripped_output" == *"/tmp/dsh-test"* ]]
}

@test "External command execution - ls" {
    run "./dsh" <<EOF
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Handling quoted spaces" {
    run "./dsh" <<EOF
echo "hello     world"
EOF
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    [[ "$stripped_output" == *"hello     world"* ]]
}

@test "Redirection - echo to file" {
    rm -f test_output.txt
    run "./dsh" <<EOF
echo "test message" > test_output.txt
EOF
    sleep 1  # Ensure file is written
    [ -f "test_output.txt" ]  
    grep -q "test message" test_output.txt  
}
