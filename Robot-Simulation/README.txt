To run on windows -
    run.bat input_filename
To run on Linux -
    bash run.sh input_filename

Add all inputs to the 'inputs/' folder, and pass only the filename as the argument
For example - to run an input file 'inputs/test_input.txt' the command on Windows would be 'run.bat test_input.txt'.

Dependencies - 

1. A C++ compiler
2. Python 3.x

Possible Improvements - 

1. The algorithm was found to not work correctly for some inputs when run with optimization mode on. In optimization
mode, if we have already explored all possible strategies from a node 'v', we store them in a suitable data structure.
In the future while running the algorithm from another node 'u', if we reach the node 'v', instead of re-exploring all
paths from 'v' we simply append thid information to the current information. Some debugging within the exploreAllPathsUtil
function in the condition block concerning optimization will be required for the same.

2. All states are enumerated in the beginning and then the search space is narrowed down by only limiting the state space
to the safe set. It was observed that usually the state space is very large as compared to size of the safe set. Thus, 
instead of enumberating all states a recursive approach for creating all states starting from the start state, moving outwards
step by step until k steps, could be applied. This would save computation time and space used.

3. Extending the game definition by adding a target co-ordinate for Robot 2 and seeing if it is possible for both of the robots
to achieve their respective objectives without colliding with each others or an obstacle within k steps.

4. Currently, the code accepts the state wherein both the robots simultaneously reach the target node of Robot 1 as a valid
terminating move. This could be tweaked if such a formulation is not desirable.

5. Currently, the data structure pw and minPW within the class Game are hash maps which take as key a concatenation of the
identifiers of all the player 2 nodes present within them. This could lead to large key lengths and sold be optimized.
