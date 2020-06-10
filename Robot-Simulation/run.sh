# python3 input_generator.py
input_file=$1
g++ robotSimulation.cpp
./a.out < ./inputs/$input_file
python3 visualize.py ./inputs/$input_file ./outputs/minimal_pws.txt
