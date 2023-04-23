cd /home/csgrad/kchand/cse489589_assignment2/kchand
make clean
make
cd /home/csgrad/kchand/cse489589_assignment2/grader

# Prompt the user to enter a value for the -t option
read -p "Enter a value for the -p option: " p_option

# Check if the input matches the desired words
if [ "$p_option" == "abt" ] || [ "$p_option" == "gbn" ] || [ "$p_option" == "sr" ]; then
    ./advanced_tests -p  ./../kchand/$p_option -r ./run_experiments
else
    # Print an error message
    echo "Error: Invalid input for -p option"
fi

# Navigate back to the original directory
cd ../