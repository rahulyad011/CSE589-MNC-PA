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

# Run commands and extract the last line of output


# PA1 Author
 author=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "author" | tail -n 1)
echo -e "\033[33m author: $author/TRUE\033[0m"

# PA1 Stage 1
 startup=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "startup" | tail -n 1)
echo -e "\033[33m startup: $startup/5.0\033[0m"
 ip=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "ip" | tail -n 1)
echo -e "\033[33m ip: $ip/5.0\033[0m"
 port=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "port" | tail -n 1)
echo -e "\033[33m port: $port/2.5\033[0m"
 list=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "_list" | tail -n 1)
echo -e "\033[33m list: $list/10.0\033[0m"
 refresh=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "refresh" | tail -n 1)
echo -e "\033[33m refresh: $refresh/5.0\033[0m"
 exit=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "exit" | tail -n 1)
echo -e "\033[33m exit: $exit/2.5\033[0m"
 exception_login=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "exception_login" | tail -n 1)
echo -e "\033[33m exception_login: $exception_login/2.0\033[0m"

# PA1 Stage 2
 send=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "send" | tail -n 1)
echo -e "\033[33m send: $send/15.0\033[0m"
 broadcast=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "broadcast" | tail -n 1)
echo -e "\033[33m broadcast: $broadcast/10.0\033[0m"
 statistics=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "statistics" | tail -n 1)
echo -e "\033[33m statistics: $statistics/5.0\033[0m"
 block=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "block" | tail -n 1)
echo -e "\033[33m block: $block/5.0\033[0m"
 blocked=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "blocked" | tail -n 1)
echo -e "\033[33m blocked: $blocked/5.0\033[0m"
 unblock=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "unblock" | tail -n 1)
echo -e "\033[33m unblock: $unblock/2.5\033[0m"
 buffer=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "buffer" | tail -n 1)
echo -e "\033[33m buffer: $buffer/5.0\033[0m"
 logout=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "logout" | tail -n 1)
echo -e "\033[33m logout: $logout/2.5\033[0m"
 exception_send=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "exception_send" | tail -n 1)
echo -e "\033[33m exception_send: $exception_send/2.0\033[0m"
 exception_block=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "exception_block" | tail -n 1)
echo -e "\033[33m exception_block: $exception_block/2.0\033[0m"
 exception_blocked=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "exception_blocked" | tail -n 1)
echo -e "\033[33m exception_blocked: $exception_blocked/2.0\033[0m"
 exception_unblock=$(./grader_controller -c grader.cfg -s "../kchand_pa1.tar" -t "exception_unblock" | tail -n 1)
echo -e "\033[33m exception_unblock: $exception_unblock/2.0\033[0m"

# Print the output
echo ""
echo "OUTPUT:"
echo ""

# Author
echo -e "\033[33m author: $author/TRUE\033[0m"

# Stage 1
echo "Stage 1:"
echo -e "\033[31m startup: $startup/5.0\033[0m"
echo -e "\033[32m ip: $ip/5.0\033[0m"
echo -e "\033[30m port: $port/2.5\033[0m"
echo -e "\033[34m list: $list/10.0\033[0m"
echo -e "\033[35m refresh: $refresh/5.0\033[0m"
echo -e "\033[36m exit: $exit/2.5\033[0m"
echo -e "\033[38;5;202m exception_login: $exception_login/2.0\033[0m"

# Stage 2
echo "Stage 2:"
echo -e "\033[31m send: $send/15.0\033[0m"
echo -e "\033[32m broadcast: $broadcast/10.0\033[0m"
echo -e "\033[30m statistics: $statistics/5.0\033[0m"
echo -e "\033[34m block: $block/5.0\033[0m"
echo -e "\033[35m blocked: $blocked/5.0\033[0m"
echo -e "\033[36m unblock: $unblock/2.5\033[0m"
echo -e "\033[38;5;202m buffer: $buffer/5.0\033[0m"
echo -e "\033[32m logout: $logout/2.5\033[0m"
echo -e "\033[30m exception_send: $exception_send/2.0\033[0m"
echo -e "\033[34m exception_block: $exception_block/2.0\033[0m"
echo -e "\033[35m exception_blocked: $exception_blocked/2.0\033[0m"
echo -e "\033[36m exception_unblock: $exception_unblock/2.0\033[0m"

# Navigate back to the original directory
cd ../
