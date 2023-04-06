#!/bin/bash

# Navigate to the directory containing the makefile
cd /home/csgrad/kchand/cse489589_assignment1/kchand

# Clear the terminal
clear

# Clean the project
make clean

# Build the project
make

# Navigate up one level
cd ..

# Execute the package script and provide "kchand" as input
echo "kchand" | ./assignment1_package.sh

# Navigate to the grader directory
cd grader/

# Prompt the user to enter a value for the -t option
read -p "Enter a value for the -t option: " t_option

# Check if the input matches the desired words
if [ "$t_option" == "author" ] || [ "$t_option" == "startup" ] || [ "$t_option" == "ip" ] || [ "$t_option" == "port" ] || [ "$t_option" == "_list" ] || [ "$t_option" == "refresh" ] || [ "$t_option" == "exit" ] || [ "$t_option" == "exception_login" ] || [ "$t_option" == "send" ] || [ "$t_option" == "broadcast" ] || [ "$t_option" == "statistics" ] || [ "$t_option" == "block" ] || [ "$t_option" == "blocked" ] || [ "$t_option" == "unblock" ] || [ "$t_option" == "buffer" ] || [ "$t_option" == "logout" ] || [ "$t_option" == "exception_send" ] || [ "$t_option" == "exception_block" ] || [ "$t_option" == "exception_blocked" ] || [ "$t_option" == "exception_unblock" ]; then
    # Start the grader controller with the specified configuration, package file, and -t option
    ./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "$t_option"
else
    # Print an error message
    echo "Error: Invalid input for -t option"
fi

# Navigate back to the original directory
cd ../
