# include <iostream>
# include <vector>
# include <list>
# include <unordered_map>
# include <unordered_set>
# include <algorithm>
# include <string.h>
# include <ctime>
#include <iostream>
#include <fstream>

using namespace std;

/*
    Class PW is the data structure used to to store Potentially Winning Sets.
    'nodes'is a list of Player 2 nodes on the PW path 
    'successor' is the immediate successor on the PW path 
    'isCycle' is a boolean which denotes whether the PW set is due to a cycle or a lasso/winning set
*/
class PW {
    public:
        list <int> nodes;
        int successor;
        bool isCycle;

        PW(list <int> player2Nodes, int succ, bool c){
            nodes = player2Nodes;
            successor = succ;
            isCycle = c;
        }
};

/*
    Class Node is used to represent a state. 
    'isSafe' is a boolean denoting if the node belongs to the safe set
    'isPlayer1' is a boolean denoting if the node is a Player 1 node
    'isWinning' is a boolean denoting if the node belongs to the winning set
    'pws' is a hashmap mapping from a set of state descriptors to a potentially winning set containing those states
*/
class Node {
    public:
        bool isSafe;
        bool isPlayer1;
        bool isWinning;
        unordered_map <string, PW> pws;

        Node(bool s, bool p){
            isSafe = s;
            isPlayer1 = p;
            isWinning = false;
        }
};

/*
    Game is the data structure used to denote the complete 2-player game. 
    'vertices' and 'edges' store the number of vertices and edges in the graph
    'nodeMap' maps state ids to actual states (Nodes)
    'adj' is the adjacency list for the graph
    'predecessorList' stores for a node the list of its predecessors
    'successorCount' stores for a node the number of successors that it has
    'winningSet' stores the nodes belonging to the winning set while 'notWinningSet' stores its complement
    'unSafeSet' stores the complement of the nodes belonging to the safe set
    'player1Nodes' is the set of all Player 1 nodes
    'pws' and 'minPWs' stores all potentially winning paths and minimal potentially winning paths
    'explored' is the set of nodes starting from which all admissible strategies have been already explored
    'optimization is a boolean denoting if the algorithm is to be run in optimization mode
*/
class Game{
    public:
        int vertices;
        int edges;
        unordered_map <int, Node*> nodeMap;
        list <int> *adj;
        list <int> *predecessorList; 
        int *successorCount;

        unordered_set<int> winningSet;
        unordered_set<int> notWinningSet;
        unordered_set<int> unSafeSet;
        unordered_set<int> player1Nodes;

        unordered_map <int, unordered_map <string, PW> > pws;
        unordered_map <int, unordered_map <string, PW> > minPWs;

        unordered_set <int> explored;
        bool optimization;

        Game(int v, int e, bool opt){
            vertices = v;
            edges = e;
            optimization = opt;
            adj = new list<int>[v];
            predecessorList = new list<int>[v];
            successorCount=new int[v];
            memset(successorCount, 0, sizeof(successorCount));
        }

        // Creates a new node
        void setNode(int id, bool isSafe, bool isPlayer1){
            nodeMap[id] = new Node(isSafe, isPlayer1);
            if(!isSafe){
                unSafeSet.insert(id);
            }
            if(isPlayer1){
                player1Nodes.insert(id);
            }
        }

        // Adds an edge between 2 nodes
        void addMove(int u, int v){
            adj[u].push_back(v);
        }

        // Populates data structures required for reverse DFS and initiates it starting from all nodes in the unsafe set
        void initializePropagate(){
            for(int i=0; i<vertices; i++){
                for(int j: adj[i]){
                    predecessorList[j].push_back(i);
                    successorCount[i]++;
                }
            }
            for(int i: unSafeSet){
                propagate(i);
            }
        }

        // Uses reverse DFS to check if Robot 2 can force the game out of the safe set
        void propagate(int v){
            if(notWinningSet.find(v) != notWinningSet.end()){
                return;
            }
            notWinningSet.insert(v);
            for(int u: predecessorList[v]){
                successorCount[u]--;
                if(player1Nodes.find(u) == player1Nodes.end() || successorCount[u] == 0){
                    propagate(u);
                }
            }
        }

        // Winning set is the complement of the notWinning set which is the set from which an unsafe node is reachable
        void findWinningSet(){
            for(int i=0;i<vertices;i++){
                if(notWinningSet.find(i)==notWinningSet.end()){
                    winningSet.insert(i);
                }
            }
            notWinningSet.clear();
        }
        
        // A node is potentially winning if it does not belong to the winning set
        bool checkPotentiallyWinning(list <int> nodes){
            for(int i: nodes){
                if(winningSet.find(i) == winningSet.end()){
                    return true;
                }
            }
            return false;
        }

        // Explores all paths starting from state 'u'
        void exploreAllPathsUtil(int u, int start, Node *node, vector <int> &visited, list <int> &currentPlayer2Nodes, int successor, list <int> &path){
            if(visited[u]) return;
            visited[u] = 1;
            // For all neighbours of 'u'
            for(int v: adj[u]){
                // If program is run using optimization, and we reach a node starting from where the Potentially Winning sets have 
                // been already been computed, then we append the PW set information of this node to the current information without
                // re-exploring all paths from this node.
                if(optimization && explored.find(v) != explored.end()){
                    Node *node1 = nodeMap[v];
                    if(!node1->isPlayer1) currentPlayer2Nodes.push_back(v);
                    for(pair<string, PW> x: minPWs[v]) {
                        bool isCycle = false;
                        list <int> temp(currentPlayer2Nodes.begin(), currentPlayer2Nodes.end());
                        string s = "";
                        unordered_set <int> nodes;
                        for(int j: temp){
                            nodes.insert(j);
                            s += to_string(j) + "~";
                        }
                        if(x.second.nodes.size() != 0){
                            bool flag = true;
                            for(int k: x.second.nodes){
                                if(k == v) {
                                    if(x.second.isCycle) isCycle = true;
                                    else flag = false;
                                    break;
                                }
                                if(nodes.find(k) != nodes.end()){
                                    flag = false;
                                    break;
                                }
                                s += to_string(k) + "~";
                                temp.push_back(k);
                            }
                            if(flag && checkPotentiallyWinning(temp)){
                                PW *pw = new PW(temp, successor, isCycle);
                                s += to_string(pw->successor); 
                                if(node->pws.find(s) == node->pws.end()) {
                                    node->pws.insert({s, *pw});
                                } 
                            }
                        }
                    }
                    if(!node1->isPlayer1) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                    continue;
                }
                // If the node is unsafe, terminate this branch of DFS
                if(unSafeSet.find(v) != unSafeSet.end()){
                    continue;
                }
                // If the node belongs to the winning set, add a new potentially winning set
                else if(winningSet.find(v) != winningSet.end()){
                    Node* node1 = nodeMap[v];
                    string s = "";
                    if(!node1->isPlayer1) currentPlayer2Nodes.push_back(v);
                    for(int i: currentPlayer2Nodes){
                        s += to_string(i) + "~";
                    }
                    if(currentPlayer2Nodes.size() != 0 && checkPotentiallyWinning(currentPlayer2Nodes)) {
                        PW *pw = new PW(currentPlayer2Nodes, successor, false);
                        s += to_string(pw->successor);
                        if(node->pws.find(s) == node->pws.end()){
                            node->pws.insert({s, *pw});
                        }
                    }
                    if(!node1->isPlayer1) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                }
                // If a new node has reached, apply algorithm recursivley to this node
                else if(visited[v] != 1){
                    Node *node1 = nodeMap[v];
                    path.push_back(v);
                    if(!node1->isPlayer1){
                        currentPlayer2Nodes.push_back(v);
                    }
                    exploreAllPathsUtil(v, start, node, visited, currentPlayer2Nodes, successor, path);
                    if(!node1->isPlayer1) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                    path.erase(prev(path.end()));
                }
                // If node is already on the DFS stack, i.e a cycle/lasso has been found, add a new Potentially Winning set
                else{
                    string s = "";
                    for(int i: currentPlayer2Nodes){
                        s += to_string(i) + "~";
                    }
                    if(currentPlayer2Nodes.size() != 0 && checkPotentiallyWinning(currentPlayer2Nodes)) {
                        bool isCycle = false;
                        if(v == start) isCycle = true;
                        PW *pw = new PW(currentPlayer2Nodes, successor, isCycle);
                        s += to_string(pw->successor);
                        if(node->pws.find(s) == node->pws.end()){
                            node->pws.insert({s, *pw});
                        }
                    } 
                }
            }
            visited[u] = 0;
        }

        // Explore all paths starting from the state 'u'
        void exploreAllPaths(int u){
            Node *node = nodeMap[u];
            // 'path' and cureentPlayer2Nodes store a list of all nodes and all player 2 nodes currently on the DFS stack respectively
            list <int> currentPlayer2Nodes, path;
            path.push_back(u);
            if(!node->isPlayer1) currentPlayer2Nodes.push_back(u);
            vector <int> visited(vertices, 0);
            visited[u] = 1;
            // Applies algorithm recursively to all of u's neighbours
            for(int v: adj[u]){
                if(unSafeSet.find(v) != unSafeSet.end()){
                    continue;
                }
                path.push_back(v);
                Node *node1 = nodeMap[v];
                if(!node1->isPlayer1) currentPlayer2Nodes.push_back(v);
                exploreAllPathsUtil(v, u, node, visited, currentPlayer2Nodes, v, path);
                if(!node1->isPlayer1) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                path.erase(prev(path.end()));
            }
            // Outputs all potentially winning paths from the start node
            cout << "Potentially Winning Sets from node " << u << endl;
            if(node->pws.size() != 0){
                for(pair<string, PW> x: node->pws) {
                    if(x.second.isCycle) cout << "Cycle: ";
                    else cout << "Lasso: ";
                    for(int j: x.second.nodes){
                        cout << j << " ";
                    }
                    cout << endl;
                }
            }
            else cout << "None" << endl;
        }

        // Computes minimal PW sets by comparingall PW sets with each other for minimality
        void getMinimalPWs(int u){
            for(pair<string, PW> x: nodeMap[u]->pws) {
                bool isCycle = x.second.isCycle;
                bool check = true;
                string s = "";
                for(int i: x.second.nodes){
                    s += to_string(i) + "~";
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
                        if(*it == *it1){
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
                    if(minPWs[u].find(s) == minPWs[u].end()){
                        minPWs[u].insert({s, x.second});
                    }
                }
            }
        }
};

// Output the time taken to run the program to the file 'outputs/runtimes.txt' and the cmd
void get_runtime(clock_t startTime){
    clock_t endTime = clock();
    clock_t clockTicksTaken = endTime - startTime;
    double timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
    cout << "Time taken:- " << timeInSeconds << " seconds." << endl;
    
    ofstream outfile;
    outfile.open("outputs/runtimes.txt", ios_base::app);
    outfile << timeInSeconds << endl;
}

int main(int argc, char *argv[]){
    // Start clock to measure time
    clock_t startTime = clock();
    // Take cmd-line argument for running algorithm with optimization 
    bool optimization = false;
    if(argc == 2) optimization = true;

    // Read input describing problem instance. See inputs/input_format.txt for input format.
    int v, e;
    cin >> v >> e;
    Game game(v, e, optimization);

    // Read in and create new nodes
    for(int i=0; i<v; i++){
        int id, p, s;
        bool player, isSafe = false;
        cin >> id >> p >> s;
        if (p == 1) player = true;
        else if (p == 2) player = false;
        if(s) isSafe = true;
        game.setNode(id, isSafe, player);
    }

    // Add edges between nodes
    for(int i=0; i<e; i++){
        int node1, node2;
        cin >> node1 >> node2;
        game.addMove(node1, node2);
    }

    // Finding the winning set by eliminating nodes for which a node in the unsafe set is reachable
    game.initializePropagate();
    game.findWinningSet();

    // Finding admissible strategies from all Player 1 nodes
    for(int i=0; i<v; i++){
        Node *node = game.nodeMap[i];
        if(node->isPlayer1 && node->isSafe && game.winningSet.find(i) == game.winningSet.end()){
            game.exploreAllPaths(i);
            game.getMinimalPWs(i);
            game.explored.insert(i);
        } 
        else if(game.winningSet.find(i) != game.winningSet.end()){
            cout << "Player 1 has a definite Winning Strategy!" << endl;
            get_runtime(startTime);
            return 0;
        }
    }

    // Output the admissble strategies to the cmd
    cout << "Minimal Potential Winning Sets" << endl;
    if (game.minPWs.size() != 0){
        for(pair<int, unordered_map<string, PW>> x: game.minPWs) {
            cout << "Vertex " << x.first << endl;
            for(pair<string, PW> y: x.second) {
                if(y.second.isCycle) cout << "Cycle: ";
                else cout << "Lasso: ";
                if(y.second.nodes.size() != 0){
                    for(int k: y.second.nodes){
                        cout << k << " ";
                    }
                    cout << endl;
                }
            }
        }
    }  
    else cout << "None" << endl;

    get_runtime(startTime);
}