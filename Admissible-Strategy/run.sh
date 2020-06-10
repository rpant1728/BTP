# Creates a random graph with graph size given as user input and returns all Player 1 admissible strategies for it

graph_size=$1
pgsolver-master/bin/randomgame $graph_size 1 2 5 > outputs/output.txt
python3 convert.py < outputs/output.txt > inputs/random_input.txt
g++ admissibleStrategy.cpp
./a.out < inputs/random_input.txt