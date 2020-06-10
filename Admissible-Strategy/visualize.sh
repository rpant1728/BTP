# Creates a visual representation of the random graph created by 'pgsolver'
pgsolver-master/bin/pgsolver -d outputs/graph.dot < outputs/output.txt
dotty outputs/graph.dot