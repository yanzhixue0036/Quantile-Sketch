#!/bin/bash

# Path to the compiled C++ program
program="./DPCCR"
program_file="params_4.txt"
# Check if params.txt exists
if [ ! -f "$program_file" ]; then
    echo "$program_file file not found!"
    exit 1
fi

# Loop through each line in params.txt and run the program in the background
while IFS= read -r param; do
    echo "Starting: $program $param"
    $program $param &
done < "$program_file"

# Wait for all background jobs to finish
wait
