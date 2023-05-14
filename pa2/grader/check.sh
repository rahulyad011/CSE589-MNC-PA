#!/bin/bash

echo "Execution error in:"
for file in ./log/err-*; do
    if [ -s "$file" ]; then
        echo "$file"
    fi
done

echo "Incomplete output in:"
count=0
for file in ./out/out-*; do
    if [ $(wc -l < "$file") -lt 11 ]; then
        echo "$file"
    fi
    count=$((count + 1))
done

echo "Number of output files: $count"