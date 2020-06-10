# Given an input graph size and number of times that the code has to be run, it returns the average runtime of the 
# C++ program used to compute all possible admissible strategies. The C++ code is run with and without optimization
# (not re-exploring all paths from a vertex for which the Potentially Winning sets have been computed) for each input graph.

graph_size=$1
iterations=$2
touch outputs/runtimes.txt
for (( c=1; c<=$iterations; c++ ))
do  
    pgsolver-master/bin/randomgame $graph_size 1 2 5 > outputs/output.txt
    python3 convert.py < outputs/output.txt > inputs/random_input.txt
    g++ admissibleStrategy.cpp
    ./a.out opti < inputs/random_input.txt
    ./a.out < inputs/random_input.txt
done
python3 average.py
rm outputs/runtimes.txt

