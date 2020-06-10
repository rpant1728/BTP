# Creates a visual representation of the random graph created by 'pgsolver' whose description is present in 'outputs/graph.dot'
pgsolver-master/bin/pgsolver -d outputs/graph.dot < outputs/output.txt
dotty outputs/graph.dot

# Player 1 nodes are shwn with parallelograms while Player 2 nodes are shown with rectangles
# Each node is represented by the vertex number and 0/1 (safe/unsafe) separated by a ':'.