# include <iostream>
# include <vector>
# include <list>
# include <unordered_map>
# include <unordered_set>
# include <algorithm>
# include <string.h>

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
        vector <PW> pws;
        unordered_set <string> uniquePWs;

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
        int *successorCount;
        vector <PW> minPWs;
        unordered_set <int> explored;

        unordered_set<int> notWinningSet;
        unordered_set<int> winningSet;
        unordered_set<int> player1Nodes;
        unordered_set<int> unSafeSet;
        unordered_set <string> uniqueMinPW;

        Game(int v, int e){
            vertices = v;
            edges = e;
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
        

        void exploreAllPathsUtil(int u, int start, Node *node, vector <int> &visited, list <int> &currentPlayer2Nodes, int successor, list <int> &path){
            if(visited[u]) return;
            visited[u] = 1;
            // cout << "Path: ";
            // for(int i: path){
            //     cout << i << " ";
            // }
            // cout << endl;
            for(int v: adj[u]){
                if(explored.find(v) != explored.end()){
                    
                }
                if(unSafeSet.find(v) != unSafeSet.end()){
                    visited[u] = 0;
                    return;
                }
                if(winningSet.find(v) != winningSet.end()){
                    Node* node1 = nodeMap[v];
                    string s = "";
                    if(!node1->isPlayer1) currentPlayer2Nodes.push_back(v);
                    for(int i: currentPlayer2Nodes){
                        s += to_string(i) + "~";
                    }
                    if(currentPlayer2Nodes.size() != 0 && node->uniquePWs.find(s) == node->uniquePWs.end()) {
                        PW *pw = new PW(currentPlayer2Nodes, successor, false);
                        node->uniquePWs.insert(s);
                        node->pws.push_back(*pw);
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
                    if(currentPlayer2Nodes.size() != 0 && node->uniquePWs.find(s) == node->uniquePWs.end()) {
                        bool isCycle = false;
                        if(v == start) isCycle = true;
                        PW *pw = new PW(currentPlayer2Nodes, successor, isCycle);
                        node->uniquePWs.insert(s);
                        node->pws.push_back(*pw);
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
                path.push_back(v);
                Node *node1 = nodeMap[v];
                if(!node1->isPlayer1) currentPlayer2Nodes.push_back(v);
                exploreAllPathsUtil(v, u, node, visited, currentPlayer2Nodes, v, path);
                if(!node1->isPlayer1) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                path.erase(prev(path.end()));
            }
            cout << "Potentially Winning Sets from node " << u << endl;
            if(node->pws.size() != 0){
                for(int i=0; i<node->pws.size(); i++){
                    for(int j: node->pws[i].nodes){
                        cout << j << " ";
                    }
                    cout << endl;
                }
            }
            else cout << "None" << endl;
        }

        void getMinimalPWs(int u){
            for(int i=0; i<nodeMap[u]->pws.size(); i++){
                bool isCycle = nodeMap[u]->pws[i].isCycle;
                bool check = false;
                string s = "";
                for(int i: nodeMap[u]->pws[i].nodes){
                    s += to_string(i) + "~";
                    if(winningSet.find(i) == winningSet.end()){
                        check = true;
                    }
                }
                if(!check) continue;
                for(int j=0; j<nodeMap[u]->pws.size(); j++){
                    if(j == i || nodeMap[u]->pws[i].nodes.size() <= nodeMap[u]->pws[j].nodes.size() 
                            || !(nodeMap[u]->pws[i].isCycle && nodeMap[u]->pws[j].isCycle)) {
                        continue;
                    }
                    
                    auto it = nodeMap[u]->pws[i].nodes.begin(); 
                    auto it1 = nodeMap[u]->pws[j].nodes.begin();
                    auto end = nodeMap[u]->pws[i].nodes.end();
                    auto end1 = nodeMap[u]->pws[j].nodes.end();

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
                    if(uniqueMinPW.find(s) == uniqueMinPW.end()){
                        uniqueMinPW.insert(s);
                        minPWs.push_back(nodeMap[u]->pws[i]);
                    }
                }
            }
        }
};

int main(){
    int v, e;
    cin >> v >> e;
    Game game(v, e);

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

    for(int i=0; i<v; i++){
        Node *node = game.nodeMap[i];
        if(node->isPlayer1){
            game.exploreAllPaths(i);
            game.getMinimalPWs(i);
            game.explored.insert(i);

        } 
    }

    cout << "Minimal Potential Winning Sets" << endl;
    if (game.minPWs.size() != 0){
        for(int i=0; i<game.minPWs.size(); i++){
            if(game.minPWs[i].nodes.size() != 0){
                for(int j: game.minPWs[i].nodes){
                    cout << j << " ";
                }
                cout << endl;
            }
        }
    }  
    else cout << "None" << endl;
}