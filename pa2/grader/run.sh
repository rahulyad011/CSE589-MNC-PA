#!/bin/bash

# squeue -u kchand -o "%.18i %.100j" | awk '/job-*/ {print $1}' | xargs scancel

echo 'Start executing experiment A'

exp="A"

protocol=("abt" "gbn" "sr")
loss_probs=(0.1 0.2 0.4 0.6 0.8)
window_sizes=(10 50)

for cmd in "${protocol[@]}"
do
    for loss in "${loss_probs[@]}"
    do
        for window in "${window_sizes[@]}"
        do
            csv="./out/out-$cmd-$exp-L-$loss-W-$window.csv"
            sbatch --job-name="job-$exp-L-$loss-W-$window" --output=./log/log-$cmd-$exp-L-$loss-W-$window.out --error=./log/err-$cmd-$exp-L-$loss-W-$window.err slurm.sh $csv $loss $window $cmd
        done
    done
done

echo 'Start executing experiment B'

exp="B"

protocol=("abt" "gbn" "sr")
loss_probs=(0.2 0.5 0.8)
window_sizes=(10 50 100 200 500)

for cmd in "${protocol[@]}"
do
    for loss in "${loss_probs[@]}"
    do
        for window in "${window_sizes[@]}"
        do
            csv="./out/out-$cmd-$exp-L-$loss-W-$window.csv"
            sbatch --job-name="job-$exp-L-$loss-W-$window" --output=./log/log-$cmd-$exp-L-$loss-W-$window.out --error=./log/err-$cmd-$exp-L-$loss-W-$window.err slurm.sh $csv $loss $window $cmd
        done
    done
done

echo 'All jobs related to exp A & exp B submitted'