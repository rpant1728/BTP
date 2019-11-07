pgsolver-master/bin/randomgame 10 1 2 5 > outputs/output.txt
python3 convert.py < outputs/output.txt > inputs/random_input.txt
g++ winningStrategy.cpp
./a.out < inputs/random_input.txt