#!/bin/bash
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=06:00:00
#SBATCH --partition=general-compute
#SBATCH --qos=general-compute

# Run the binary file with the given parameters
./run_experiments -p ./../kchand/$4 -o $1 -m 1000 -t 50 -c 0.2 -l $2 -w $3