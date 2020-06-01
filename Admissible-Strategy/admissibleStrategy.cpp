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

class Game{
    public:
        int vertices;
        int edges;
        unordered_map <int, Node*> nodeMap;
        list <int> *adj;
        list <int> *predecessorList; 
        bool optimization;
        int *successorCount;
        unordered_map <int, unordered_map <string, PW> > pws;
        unordered_map <int, unordered_map <string, PW> > minPWs;
        unordered_set <int> explored;

        unordered_set<int> notWinningSet;
        unordered_set<int> winningSet;
        unordered_set<int> player1Nodes;
        unordered_set<int> unSafeSet;

        Game(int v, int e, bool opt){
            vertices = v;
            edges = e;
            optimization = opt;
            adj = new list<int>[v];
            predecessorList = new list<int>[v];
            successorCount=new int[v];
            memset(successorCount, 0, sizeof(successorCount));
        }

        void setNode(int id, bool isSafe, bool isPlayer1){
            nodeMap[id] = new Node(isSafe, isPlayer1);
            if(!isSafe){
                unSafeSet.insert(id);
            }
            if(isPlayer1){
                player1Nodes.insert(id);
            }
        }

        void addMove(int u, int v){
            adj[u].push_back(v);
        }

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

        void findWinningSet(){
            for(int i=0;i<vertices;i++){
                if(notWinningSet.find(i)==notWinningSet.end()){
                    winningSet.insert(i);
                }
            }
        }
        
        bool checkPotentiallyWinning(list <int> nodes){
            for(int i: nodes){
                if(winningSet.find(i) == winningSet.end()){
                    return true;
                }
            }
            return false;
        }

        void exploreAllPathsUtil(int u, int start, Node *node, vector <int> &visited, list <int> &currentPlayer2Nodes, int successor, list <int> &path){
            if(visited[u]) return;
            visited[u] = 1;
            // cout << "Path: ";
            // for(int i: path){
            //     cout << i << " ";
            // }
            // cout << endl;
            for(int v: adj[u]){
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
                if(unSafeSet.find(v) != unSafeSet.end()){
                    continue;
                }
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

        void exploreAllPaths(int u){
            Node *node = nodeMap[u];
            list <int> currentPlayer2Nodes, path;
            path.push_back(u);
            if(!node->isPlayer1) currentPlayer2Nodes.push_back(u);
            vector <int> visited(vertices, 0);
            visited[u] = 1;
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

int main(int argc, char *argv[]){
    clock_t startTime = clock();
    bool optimization = false;
    if(argc == 2) optimization = true;

    int v, e;
    cin >> v >> e;
    Game game(v, e, optimization);

    for(int i=0; i<v; i++){
        int id, p, s;
        bool player, isSafe = false;
        cin >> id >> p >> s;
        if (p == 1) player = true;
        else if (p == 2) player = false;
        if(s) isSafe = true;
        game.setNode(id, isSafe, player);
    }

    for(int i=0; i<e; i++){
        int node1, node2;
        cin >> node1 >> node2;
        game.addMove(node1, node2);
    }

    game.initializePropagate();
    game.findWinningSet();

    cout << "Winning Set" << endl;
    for(int i: game.winningSet){
        cout << i << " ";
    }  
    cout << endl;
    cout << "Unsafe Set" << endl;
    for(int i: game.unSafeSet){
        cout << i << " ";
    }  
    cout << endl;
    int count = 0;
    for(int i=0; i<v; i++){
        Node *node = game.nodeMap[i];
        if(node->isPlayer1 && node->isSafe){
            count++;
        }
    }
    cout << "Valid Player 1 nodes:- " << count << endl;

    count = 0;
    for(int i=0; i<v; i++){
        Node *node = game.nodeMap[i];
        if(node->isPlayer1 && node->isSafe && game.winningSet.find(i) == game.winningSet.end()){
            count++;
            game.exploreAllPaths(i);
            game.getMinimalPWs(i);
            game.explored.insert(i);
            if(count%10 == 0) cout << count << " explored" << endl;
        } 
    }

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
    clock_t endTime = clock();
    clock_t clockTicksTaken = endTime - startTime;
    double timeInSeconds = clockTicksTaken / (double) CLOCKS_PER_SEC;
    cout << "Time taken:- " << timeInSeconds << " seconds." << endl;
    
    ofstream outfile;
    outfile.open("outputs/runtimes.txt", ios_base::app);
    outfile << timeInSeconds << endl;
}