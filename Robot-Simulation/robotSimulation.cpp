# include <iostream>
# include <vector>
# include <list>
# include <unordered_map>
# include <unordered_set>
# include <algorithm>
# include <string.h>
# include <ctime>
# include <iostream>
# include <fstream>
# include <sstream> 

using namespace std;

/*
    Class PW is the data structure used to to store Potentially Winning Sets.
    'nodes'is a list of Player 2 nodes on the PW path 
    'successor' is the immediate successor on the PW path 
    'isCycle' is a boolean which denotes whether the PW set is due to a cycle or a lasso/winning set
    'path' is a list of all nodes on the PW path.
*/
class PW {
    public:
        list <string> nodes;
        string successor;
        bool isCycle;
        list <string> path;

        PW(list <string> player2Nodes, string succ, bool c, list <string> p){
            nodes = player2Nodes;
            successor = succ;
            isCycle = c;
            path = p;
        }
};


/*
    Class Node is used to represent a state. 
    (i1, j1) and (i2,j2) are the current co-ordinates of R1 and R2 respectively 
    'k' is the sum of the steps taken by both the robots (a simultaneous move is simulated as two separate moves, one by each player)
    'turn' denotes who makes a move next (true if R1 plays next)
    'pws' is a hashmap mapping from a set of state descriptors to a potentially winning set containing those states
    'id' is the string representation of the state made using i1,j1,i2,j2,k and a delimiter '~' to separate them
*/
class Node {
    public:
        int i1;
        int i2;
        int j1;
        int j2;
        int k;
        bool turn;
        bool isSafe;
        unordered_map <string, PW> pws;
        string id;

        Node(int _i1,int _j1,int _i2,int _j2,int _k, bool _s){
            i1 = _i1;
            i2 = _i2;
            j1 = _j1;
            j2 = _j2;
            k = _k;
            if(_k%2 == 0) turn = true;
            else turn = false;
            isSafe = _s;
            id = to_string(i1)+"~"+to_string(j1)+"~"+to_string(i2)+"~"+to_string(j2)+"~"+to_string(_k);
        }
};

// Checks if the robots' positions lie within the grid
bool isValid(int i1, int j1, int i2, int j2, int m, int n){
    if(i1 >= 0 && j1 >= 0 && i2 >= 0 && j2 >=0 && i1 < m && j1 < n && i2 < m && j2 < n){
        return true;
    }
    return false;
}


/*
    Game is the data structure used to denote the complete 2-player game. 
    'obstacles' is the hash map used to store the co-ordinates of the obstacles codified into strings
    'nodeMap' maps state ids to actual states (Nodes)
    'adj' is the adjacency list for the graph
    'predecessorList' stores for a node the list of its predecessors
    'successorCount' stores for a node the number of successors that it has
    'winningSet' stores the nodes belonging to the winning set while 'notWinningSet' stores its complement
    'safeSet' stores the nodes belonging to the safe set while 'unsafeSet' stores its complement
    'player1Nodes' is the set of all Player 1 nodes
    'pws' and 'minPWs' stores all potentially winning paths and minimal potentially winning paths
    'explored' is the set of nodes starting from which all admissible strategies have been already explored
    'optimization is a boolean denoting if the algorithm is to be run in optimization mode
*/
class Game{
    public:
        int m, n, k, p, x1, y1, x2, y2, xd, yd;
        unordered_set <string> obstacles;
        unordered_map <string, Node*> nodeMap;
        unordered_map <string, list <string> > adj;
        unordered_map <string, list <string> > predecessorList; 
        unordered_map <string, int> successorCount;

        unordered_set <string> winningSet;
        unordered_set <string> notWinningSet;
        unordered_set <string> safeSet;
        unordered_set <string> unsafeSet;
        unordered_set <string> player1Nodes;

        unordered_map <string, PW> pws;
        unordered_map <string, PW> minPWs;

        unordered_set <string> explored;
        bool optimization;

        Game(bool opt, int _m , int _n, int _k, int _p, int _x1, int _y1, int _x2, int _y2, 
                    int _x3, int _y3, unordered_set <string> _obs){
            m = _m;
            n = _n;
            k = _m;
            p = _n;
            x1 = _x1;
            y1 = _y1;
            x2 = _x2;
            y2 = _y2;
            xd = _x3;
            yd = _y3;
            obstacles = _obs;
            optimization = opt;
        }

        // Creates a new node
        void setNode(int _i1, int _j1, int _i2, int _j2, int _k, bool _s){
            string id=to_string(_i1)+"~"+to_string(_j1)+"~"+to_string(_i2)+"~"+to_string(_j2)+"~"+to_string(_k);
            if(isValid(_i1, _j1, _i2, _j2, m, n)){
                nodeMap[id] = new Node(_i1,_j1,_i2,_j2,_k, _s);
                if(_k%2 == 0){
                    player1Nodes.insert(id);
                }
                adj[id]=list<string>();
                predecessorList[id]=list<string>();
            }
        }

        // Adds an edge between 2 nodes
        void addMove(string u, string v){
            if(nodeMap.find(u)!=nodeMap.end() && nodeMap.find(u)!=nodeMap.end()){
                adj[u].push_back(v);
            }
        }

        // Creates all nodes and adds suitable edges
        void createGraph(){
            // Enumerates all states by creating all possible combinations of (i1,j1,i2,j2,k)
            for(int l=0; l<2*k; l++){
                for(int i1=0; i1<m; i1++){
                    for(int j1=0; j1<n; j1++){
                        for(int i2=0; i2<m; i2++){
                            for(int j2=0; j2<n; j2++){
                                // A vector to represent the possible future states
                                vector <pair <int, int>> possible{{0,1}, {1,0}, {-1,0}, {0,-1}};
                                string currPlayer1 = to_string(i1) + "~" + to_string(j1);
                                string currPlayer2 = to_string(i2) + "~" + to_string(j2);
                                // If either (i1, j1) or (i2, j2) is a n obstacle don't create a node
                                if(obstacles.find(currPlayer1) != obstacles.end() || obstacles.find(currPlayer2) != obstacles.end()){
                                    continue;
                                }
                                string currState = currPlayer1 + "~" + currPlayer2 + "~" + to_string(l);
                                // In case of no collison between the two robots
                                if(!(i1 == i2 && j1 == j2)){ 
                                    // Create node if not already created
                                    if(nodeMap.find(currState) == nodeMap.end()){
                                        setNode(i1,j1,i2,j2,l,true);
                                    }
                                    // If robot 1 reaches its target co-ordinate, add state to winning set and add a self loop to this state
                                    if(i1==xd && j1==yd){
                                        winningSet.insert(currState);
                                        addMove(currState, currState);
                                    }
                                    else{
                                        // If it is a Player 1 node, add a move to the neighbouring cells in the grid, don't do so if
                                        // a neighbouring cell is an obstacle or is an invalid board position. Similar pattern is
                                        // followed for a Player 2 node.
                                        if(l%2 == 0){
                                            for(int o=0;o<4; o++){
                                                string nextPlayer1 = to_string(i1+possible[o].first) + "~" + to_string(j1+possible[o].second);
                                                if(obstacles.find(nextPlayer1) == obstacles.end() && isValid(i1+possible[o].first,j1+possible[o].second,i2,j2,m,n)){
                                                    string nextState1 = nextPlayer1+"~"+currPlayer2+"~"+to_string(l+1);
                                                    if(nodeMap.find(nextState1) == nodeMap.end() && isValid(i1+possible[o].first,j1+possible[o].second,i2,j2,m,n)){
                                                        setNode(i1+possible[o].first,j1+possible[o].second,i2,j2,l+1,true);
                                                    }
                                                    addMove(currState, nextState1);
                                                }
                                            }
                                        }
                                        else{
                                            for(int o=0;o<4; o++){
                                                string nextPlayer2 = to_string(i2+possible[o].first) + "~" + to_string(j2+possible[o].second);
                                                if(obstacles.find(nextPlayer2) == obstacles.end() && isValid(i1,j1,i2+possible[o].first,j2+possible[o].second,m,n)){
                                                    string nextState2 = currPlayer1+"~"+nextPlayer2+"~"+to_string(l+1);
                                                    if(nodeMap.find(nextState2) == nodeMap.end() && isValid(i1,j1,i2+possible[o].first,j2+possible[o].second,m,n)){
                                                        setNode(i1,j1,i2+possible[o].first,j2+possible[o].second,l+1,true);
                                                    }
                                                    addMove(currState, nextState2);
                                                }
                                            }
                                        }
                                    }
                                }
                                // If it is a collision set, add it to the unsafe set
                                else{
                                    if(nodeMap.find(currState) == nodeMap.end()){
                                        setNode(i1,j1,i2,j2,l+1,false);
                                    }
                                    unsafeSet.insert(currState);
                                }
                            }
                        }
                    }
                }
            }
        }

        // Create data structures required for reverse DFS
        void initializeReverseDFS(){
            for(auto i:adj){
                for(string j: i.second){
                    predecessorList[j].push_back(i.first);
                    successorCount[i.first]++;
                }
            }
        }

        // Apply reverse DFS recursively. Add a node to the safe set if its 'k' value i.e the number of steps taken so far to reach
        // this state, and the depth of the DFS is not greater than the number of moves alllowed. This ensures that we apply our algorithm
        // only on those nodes from which it is possible for Robit 1 to reach its target position within 'k' steps
        void reverseDFS(string v, int i, int k){
            if(safeSet.find(v) != safeSet.end() || i > 2*k){
                return;
            }
            Node *node = nodeMap[v];
            if(node->k+i <= 2*k){
                safeSet.insert(v);
                nodeMap[v]->isSafe=true;
            }
            for(string u: predecessorList[v]){
                reverseDFS(u,i+1, k);
            }
        }

        // Reverse DFS is started from all nodes in the unsafe set
        void initializePropagate(){
            for(string i: unsafeSet){
                propagate(i);
            }
        }

        // Uses reverse DFS to check if Robot 2 can force the game out of the safe set
        void propagate(string v){
            if(notWinningSet.find(v) != notWinningSet.end()){
                return;
            }
            notWinningSet.insert(v);
            for(string u: predecessorList[v]){
                successorCount[u]--;
                if(player1Nodes.find(u) == player1Nodes.end() || successorCount[u] == 0){
                    propagate(u);
                }
            }
        }

        // Winning set is the complement of the notWinning set which is the set from which an unsafe node is reachable
        void findWinningSet(){
            for(auto i: adj){
                if(notWinningSet.find(i.first)==notWinningSet.end() && safeSet.find(i.first) != safeSet.end()){
                    winningSet.insert(i.first);
                }
            }
        }
        
        // A node is potentially winning if it does not belong to the winning set
        bool checkPotentiallyWinning(list <string> nodes){
            for(string i: nodes){
                if(winningSet.find(i) == winningSet.end()){
                    return true;
                }
            }
            return false;
        }

        // While comparing two state identifiers, it only compares their i1, j1, i2, j2 values, thus ignoring their k values
        // This done to ensure minimal paths while finding admissible strategies
        bool compareStrings(string state1, string state2){
            if(state1.length() != state2.length()) return false;
            int count = 0;
            for(int i=0; i<state1.length(); i++){
                if(state1[i] == '~'){
                    if(count == 3) return true;
                    count++;
                }
                if(state1[i] != state2[i]) return false;
            }
            return true;
        }

        // Explores all paths starting from state 'u'
        void exploreAllPathsUtil(string u, string start, Node *node, unordered_set <string> visited, list <string> &currentPlayer2Nodes, string successor, list <string> &path){
            if(visited.find(u) != visited.end()) return;
            visited.insert(u);
            // For all neighbours of 'u'
            for(string v: adj[u]){
                // If program is run using optimization, and we reach a node starting from where the Potentially Winning sets have 
                // been already been computed, then we append the PW set information of this node to the current information without
                // re-exploring all paths from this node.
                if(optimization && explored.find(v) != explored.end()){
                    Node *node1 = nodeMap[v];
                    if(!node1->turn) currentPlayer2Nodes.push_back(v);
                    for(pair<string, PW> x: minPWs) {
                        bool isCycle = false;
                        list <string> temp(currentPlayer2Nodes.begin(), currentPlayer2Nodes.end());
                        string s = "";
                        unordered_set <string> nodes;
                        for(string j: temp){
                            nodes.insert(j);
                            s += j + "^";
                        }
                        if(x.second.nodes.size() != 0){
                            bool flag = true;
                            for(string k: x.second.nodes){
                                if(k == v) {
                                    if(x.second.isCycle) isCycle = true;
                                    else flag = false;
                                    break;
                                }
                                if(nodes.find(k) != nodes.end()){
                                    flag = false;
                                    break;
                                }
                                s += k + "^";
                                temp.push_back(k);
                            }
                            if(flag && checkPotentiallyWinning(temp)){
                                PW *pw = new PW(temp, successor, isCycle, path);
                                s += pw->successor; 
                                if(node->pws.find(s) == node->pws.end()) {
                                    node->pws.insert({s, *pw});
                                } 
                            }
                        }
                    }
                    if(!node1->turn) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                    continue;
                }
                // If the node is unsafe, terminate this branch of DFS
                if(unsafeSet.find(v) != unsafeSet.end()){
                    continue;
                }
                // If the node belongs to the winning set, add a new potentially winning set
                else if(winningSet.find(v) != winningSet.end()){
                    Node* node1 = nodeMap[v];
                    string s = "";
                    path.push_back(v);
                    if(!node1->turn) currentPlayer2Nodes.push_back(v);
                    for(string i: currentPlayer2Nodes){
                        s += i + "^";
                    }
                    if(currentPlayer2Nodes.size() != 0 && checkPotentiallyWinning(currentPlayer2Nodes)) {
                        PW *pw = new PW(currentPlayer2Nodes, successor, false, path);
                        s += pw->successor;
                        if(node->pws.find(s) == node->pws.end()){
                            node->pws.insert({s, *pw});
                        }
                    }
                    if(!node1->turn) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                    path.erase(prev(path.end()));
                }
                // If a new node has reached, apply algorithm recursivley to this node
                else if(visited.find(v) == visited.end()){
                    Node *node1 = nodeMap[v];
                    path.push_back(v);
                    if(!node1->turn){
                        currentPlayer2Nodes.push_back(v);
                    }
                    exploreAllPathsUtil(v, start, node, visited, currentPlayer2Nodes, successor, path);
                    if(!node1->turn) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                    path.erase(prev(path.end()));
                }
                // If node is already on the DFS stack, i.e a cycle/lasso has been found, add a new Potentially Winning set
                else{
                    path.push_back(v);
                    Node* node1 = nodeMap[v];
                    if(!node1->turn) currentPlayer2Nodes.push_back(v);
                    string s = "";
                    for(string i: currentPlayer2Nodes){
                        s += i + "^";
                    }
                    if(currentPlayer2Nodes.size() != 0 && checkPotentiallyWinning(currentPlayer2Nodes)) {
                        bool isCycle = false;
                        if(v == start) isCycle = true;
                        PW *pw = new PW(currentPlayer2Nodes, successor, isCycle, path);
                        s += pw->successor;
                        if(node->pws.find(s) == node->pws.end()){
                            node->pws.insert({s, *pw});
                        }
                    }
                    if(!node1->turn) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                    path.erase(prev(path.end())); 
                }
            }
            visited.insert(u);
        }

        // Explore all paths starting from the state 'u'
        void exploreAllPaths(string u){
            Node *node = nodeMap[u];
            // 'path' and cureentPlayer2Nodes store a list of all nodes and all player 2 nodes currently on the DFS stack respectively
            list <string> currentPlayer2Nodes, path;
            path.push_back(u);
            if(!node->turn) currentPlayer2Nodes.push_back(u);
            unordered_set <string> visited;
            visited.insert(u);
            // Applies algorithm recursively to all of u's neighbours
            for(string v: adj[u]){
                if(unsafeSet.find(v) != unsafeSet.end()){
                    continue;
                }
                path.push_back(v);
                Node *node1 = nodeMap[v];
                if(!node1->turn) currentPlayer2Nodes.push_back(v);
                exploreAllPathsUtil(v, u, node, visited, currentPlayer2Nodes, v, path);
                if(!node1->turn) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                path.erase(prev(path.end()));
            }

            // Outputs all potentially winning paths from the start node
            cout << "Potentially Winning Sets from node " << u << endl;
            if(node->pws.size() != 0){
                for(pair<string, PW> x: node->pws) {
                    if(x.second.isCycle) cout << "Cycle: ";
                    else cout << "Lasso: ";
                    for(string j: x.second.nodes){
                        cout << j << " ";
                    }
                    cout << endl;
                }
            }
            else cout << "None" << endl;
        }

        // Computes minimal PW sets by comparingall PW sets with each other for minimality
        void getMinimalPWs(string u){
            for(pair<string, PW> x: nodeMap[u]->pws) {
                bool isCycle = x.second.isCycle;
                bool check = true;
                string s = "";
                for(string i: x.second.nodes){
                    s += i + "`";
                }
                for(pair<string, PW> y: nodeMap[u]->pws) {
                    if(x.first == y.first || x.second.successor != y.second.successor || x.second.nodes.size() <= y.second.nodes.size() 
                        || (x.second.isCycle && !y.second.isCycle) || (!x.second.isCycle && y.second.isCycle)) {
                        continue;
                    }

                    auto it = x.second.nodes.begin(); 
                    auto it1 = y.second.nodes.begin();
                    auto end = x.second.nodes.end();
                    auto end1 = y.second.nodes.end();

                    while(it != end && it1 != end1){
                        if(compareStrings(*it, *it1)){
                            it1++;
                        }
                        it++;
                    }
                    if(it1 == end1){                        
                        check = false;
                        break;
                    }
                }
                if(check) {
                    if(minPWs.find(s) == minPWs.end()){
                        minPWs.insert({s, x.second});
                    }
                }
            }
        }
};


int main(int argc, char *argv[]){
    // Start clock to measure time
    clock_t startTime = clock();

    // Take cmd-line argument for running algorithm with optimization 
    bool optimization = false;
    if(argc == 2) optimization = true;

    // Read input describing problem instance. See inputs/input_format.txt for input format.
    int m, n, k, p;
    cin >> m >> n >> k >> p;

    int x1, y1, x2, y2, xd, yd;
    cin >> x1 >> y1 >> x2 >> y2 >> xd >> yd;

    unordered_set <string> obstacles;

    // Store co-ordinates of obstacles in a hash map
    for(int i=0; i<p; i++){
        int x, y;
        cin >> x >> y;
        obstacles.insert(to_string(x) + "~" + to_string(y));
    }
    
    // Create a game instance using input
    Game game(optimization, m, n, k, p, x1, y1, x2, y2, xd, yd, obstacles);

    // Create graph nodes and add edges according to the transitions dictated by the problem description
    game.createGraph();    

    // Do a reverse DFS from all the nodes in the winning set to find the safe set
    game.initializeReverseDFS();
    for(auto i: game.winningSet){
        game.reverseDFS(i, 0, k);
    }

    // Add all nodes not in the safe set to the unsafe set
    for(auto i: game.nodeMap){
        if(game.safeSet.find(i.first) == game.safeSet.end()){
            game.unsafeSet.insert(i.first);
        }
    }

    // Finding the winning set by eliminating nodes for which a node in the unsafe set is reachable
    game.initializePropagate();
    game.findWinningSet();

    // If the start state belongs to the winning set, then Robot 1 has a winning strategy
    string startState = to_string(x1)+"~"+to_string(y1)+"~"+to_string(x2)+"~"+to_string(y2)+"~0";
    if(game.winningSet.find(startState) != game.winningSet.end()){
        cout << "Player 1 has a definite Winning Strategy!" << endl;
        return 0;
    }

    // Else, explore all paths starting from the start state to find all admissible strategies
    Node *node = game.nodeMap[startState];
    if(node->isSafe){
        game.exploreAllPaths(startState);
        game.getMinimalPWs(startState);
    } 

    // Output the admissble strategies to outputs/minimal_pws.txt and cmd
    ofstream outfile;
    outfile.open("outputs/minimal_pws.txt", ios::out | ios::trunc);
    cout << "Minimal Potential Winning Sets" << endl;
    if (game.minPWs.size() != 0){
        for(auto x: game.minPWs) {
            if(x.second.isCycle){
                cout << "Cycle : ";
                outfile << "Cycle : ";
            }
            else{
                cout << "Lasso : ";
                outfile << "Lasso : ";
            } 
            if(x.second.nodes.size() != 0){
                for(string k: x.second.nodes){
                    cout << k << " ";
                    outfile << k << " ";
                }
                cout << ": ";
                outfile << ": ";
            }
            if(x.second.path.size() != 0){
                for(string k: x.second.path){
                    cout << k << " ";
                    outfile << k << " ";
                }
                cout << endl;
                outfile << endl;
            }
        }
    }  
    else cout << "None" << endl;

    // Output the time taken to run the program
    clock_t endTime = clock();
    clock_t clockTicksTaken = endTime - startTime;
    double timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
    cout << "Time taken:- " << timeInSeconds << " seconds." << endl;
}