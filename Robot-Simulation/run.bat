REM python3 input_generator.py
g++ robotSimulation.cpp
a.exe < inputs/%1
python3 visualize.py inputs/%1 outputs/minimal_pws.txt