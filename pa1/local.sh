#!/bin/bash
cd /home/csgrad/kchand/cse489589_assignment1/kchand #change to the directory
clear #clear the terminal
make clean #remove previously compiled files
make #compile the code
cd .. #change to the parent directory
echo "kchand" | ./assignment1_package.sh #run the package script with input
cd kchand/ #change to the kchand directory
#./assignment1 c 5000 #run the program with parameters