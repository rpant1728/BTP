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
        list <string> nodes;
        string successor;
        bool isCycle;

        PW(list <string> player2Nodes, string succ, bool c){
            nodes = player2Nodes;
            successor = succ;
            isCycle = c;
        }
};

class Node {
    public:
        bool isSafe;
        bool isWinning;
        int i1;
        int i2;
        int j1;
        int j2;
        int k;
        bool turn;
        unordered_map <string, PW> pws;
        string id;

        Node(int _i1,int _i2,int _j1,int _j2,int _k, bool _t){
            i1 = _i1;
            i2 = _i2;
            j1 = _j1;
            j2 = _j2;
            k = _k;
            turn = _t;
            isSafe = false;
            isWinning = false;
            string s = to_string(i1)+"~"+to_string(j1)+"~"+to_string(i2)+"~"+to_string(j2)+"~"+to_string(_k)+"~";
            if(_t) id = s+"0";
            else id = s+"1";
        }
};

class Game{
    public:
        unordered_map <string, Node*> nodeMap;
        unordered_map <string, list <string> > adj;

        unordered_map <string, list <string> > predecessorList; 
        unordered_map <string, int> successorCount;

        unordered_set <string> notWinningSet;
        unordered_set <string> winningSet;
        unordered_set <string> player1Nodes;
        unordered_set <string> safeSet;

        unordered_map <string, PW> pws;
        unordered_map <string, PW> minPWs;

        unordered_set <string> explored;
        bool optimization;

        Game(bool opt){
            optimization = opt;
        }

        void setNode( int _i1,int _i2,int _j1,int _j2, int _k, bool _turn){
            string id=to_string(_i1)+"~"+to_string(_j1)+"~"+to_string(_i2)+"~"+to_string(_j2)+"~"+to_string(_k)+"~"+to_string(_turn);
            nodeMap[id] = new Node(_i1,_i2,_j1,_j2,_k, _turn);
            // if(!isSafe){******************
            //     unSafeSet.insert(id);
            // }
            if(_turn){
                player1Nodes.insert(id);
            }
            adj[id]=list<string>();
            predecessorList[id]=list<string>();
        }

        void addMove(string u, string v){
            adj[u].push_back(v);
        }

        void BFS(string s){ 
            list <string> queue; 
            unordered_map <string, bool> visited;
            for(auto x: nodeMap){
                visited.insert({x.first, false});
            }
        
            visited[s] = true; 
            queue.push_back(s); 
        
            while(!queue.empty()){ 
                s = queue.front(); 
                queue.pop_front(); 
                for(auto i: adj[s]){
                    if (!visited[i]){ 
                        visited[i] = true; 
                        safeSet.insert(i);
                        queue.push_back(i); 
                    } 
                } 
            } 
        } 

        void initializePropagate(){
            for(auto i:adj){
                for(string j: i.second){
                    predecessorList[j].push_back(i.first);
                    successorCount[i.first]++;
                }
            }
            for(string i: unSafeSet){
                propagate(i);
            }
        }

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

        void findWinningSet(){
            for(auto i: adj){
                if(notWinningSet.find(i.first)==notWinningSet.end()){
                    winningSet.insert(i.first);
                }
            }
        }
        
        bool checkPotentiallyWinning(list <string> nodes){
            for(string i: nodes){
                if(winningSet.find(i) == winningSet.end()){
                    return true;
                }
            }
            return false;
        }

        void exploreAllPathsUtil(string u, string start, Node *node, unordered_set <string> &visited, list <string> &currentPlayer2Nodes, string successor, list <string> &path){
            if(visited.find(u) != visited.end()) return;
            visited.insert(u);
            // cout << "Path: ";
            // for(int i: path){
            //     cout << i << " ";
            // }
            // cout << endl;
            for(string v: adj[u]){
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
                                PW *pw = new PW(temp, successor, isCycle);
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
                if(unSafeSet.find(v) != unSafeSet.end()){
                    continue;
                }
                else if(winningSet.find(v) != winningSet.end()){
                    Node* node1 = nodeMap[v];
                    string s = "";
                    if(!node1->turn) currentPlayer2Nodes.push_back(v);
                    for(string i: currentPlayer2Nodes){
                        s += i + "^";
                    }
                    if(currentPlayer2Nodes.size() != 0 && checkPotentiallyWinning(currentPlayer2Nodes)) {
                        PW *pw = new PW(currentPlayer2Nodes, successor, false);
                        s += pw->successor;
                        if(node->pws.find(s) == node->pws.end()){
                            node->pws.insert({s, *pw});
                        }
                    }
                    if(!node1->turn) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                }
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
                else{
                    string s = "";
                    for(string i: currentPlayer2Nodes){
                        s += i + "^";
                    }
                    if(currentPlayer2Nodes.size() != 0 && checkPotentiallyWinning(currentPlayer2Nodes)) {
                        bool isCycle = false;
                        if(v == start) isCycle = true;
                        PW *pw = new PW(currentPlayer2Nodes, successor, isCycle);
                        s += pw->successor;
                        if(node->pws.find(s) == node->pws.end()){
                            node->pws.insert({s, *pw});
                        }
                    } 
                }
            }
            visited.insert(u);
        }

        void exploreAllPaths(string u){
            Node *node = nodeMap[u];
            list <string> currentPlayer2Nodes, path;
            path.push_back(u);
            if(!node->turn) currentPlayer2Nodes.push_back(u);
            unordered_set <string> visited;
            visited.insert(u);
            for(string v: adj[u]){
                if(unSafeSet.find(v) != unSafeSet.end()){
                    continue;
                }
                path.push_back(v);
                Node *node1 = nodeMap[v];
                if(!node1->turn) currentPlayer2Nodes.push_back(v);
                exploreAllPathsUtil(v, u, node, visited, currentPlayer2Nodes, v, path);
                if(!node1->turn) currentPlayer2Nodes.erase(prev(currentPlayer2Nodes.end()));
                path.erase(prev(path.end()));
            }
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
                    if(minPWs.find(s) == minPWs.end()){
                        minPWs.insert({s, x.second});
                    }
                }
            }
        }
};

int main(int argc, char *argv[]){
    clock_t startTime = clock();
    bool optimization = false;
    if(argc == 2) optimization = true;

    int n, m, k, p;
    cin >> n >> m >> k >> p;
    unordered_set <string> obstacles;
    for(int i=0; i<p; i++){
        int x, y;
        cin >> x >> y;
        obstacles.insert(to_string(x) + "~" + to_string(y));
    }
    int x1, y1, x2, y2, x3, y3;
    cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
    
    Game game(optimization);

    for(int l=0; l<k; l++){
        for(int i1=0; i1<n; i1++){
            for(int j1=0; j1<m; j1++){
                for(int i2=0; i2<n; i2++){
                    for(int j2=0; j2<m; j2++){
                        if(i1 != i2 || j1 != j2){  //**********************
                            vector <pair <int, int>> possible{{0,1}, {1,0}, {-1,0}, {0,-1}};
                            string currPlayer1 = to_string(i1) + "~" + to_string(j1);
                            string currPlayer2 = to_string(i2) + "~" + to_string(j2);
                            if(obstacles.find(currPlayer1) != obstacles.end() || obstacles.find(currPlayer2) != obstacles.end()){
                                continue;
                            }
                            string currState1 = currPlayer1 + "~" + currPlayer2 + "~" + to_string(l) + "~0";
                            string currState2 = currPlayer1 + "~" + currPlayer2 + "~" + to_string(l) + "~1";
                            if(game.nodeMap.find(currState1) == game.nodeMap.end()){
                                game.setNode(i1,j1,i2,j2,k,false);
                            }
                            if(game.nodeMap.find(currState2) == game.nodeMap.end()){
                                game.setNode(i1,j1,i2,j2,k,true);
                            }
                            for(int o=0;o<4; o++){
                                string nextPlayer1 = to_string(i1+possible[o].first) + "~" + to_string(j1+possible[o].second);
                                if(obstacles.find(nextPlayer1) == obstacles.end()){
                                    string nextState1 = nextPlayer1+"~"+currPlayer2+"~"+to_string(l+1)+"~1";
                                    if(game.nodeMap.find(nextState1) == game.nodeMap.end()){
                                        game.setNode(i1,j1,i2,j2,k,false);
                                    }
                                    game.addMove(currState1, nextState1);
                                }
                                string nextPlayer2 = to_string(i2+possible[o].first) + "~" + to_string(j2+possible[o].second);
                                if(obstacles.find(nextPlayer2) == obstacles.end()){
                                    string nextState2 = currPlayer1+"~"+nextPlayer2+"~"+to_string(l+1)+"~0";
                                    if(game.nodeMap.find(nextState2) == game.nodeMap.end()){
                                        game.setNode(i1,j1,i2,j2,k,true);
                                    }
                                    game.addMove(currState2, nextState2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    game.initializePropagate();
    game.findWinningSet();

    cout << "Winning Set" << endl;
    for(string i: game.winningSet){
        cout << i << " ";
    }  
    cout << endl;
    cout << "Unsafe Set" << endl;
    for(string i: game.unSafeSet){
        cout << i << " ";
    }  
    cout << endl;
    int count = 0;
    for(auto n: game.nodeMap){
        Node *node = n.second;
        if(node->turn && node->isSafe){
            count++;
        }
    }

    string startState = to_string(x1)+"~"+to_string(y1)+"~"+to_string(x2)+"~"+to_string(y2)+"~0~0";
    Node *node = game.nodeMap[startState];
    if(node->isSafe && game.winningSet.find(startState) == game.winningSet.end()){
        game.exploreAllPaths(startState);
        game.getMinimalPWs(startState);
        game.explored.insert(startState);
    } 

    cout << "Minimal Potential Winning Sets" << endl;
    if (game.minPWs.size() != 0){
        for(auto x: game.minPWs) {
            cout << "Vertex " << x.first << endl;
            if(x.second.isCycle) cout << "Cycle: ";
            else cout << "Lasso: ";
            if(x.second.nodes.size() != 0){
                for(string k: x.second.nodes){
                    cout << k << " ";
                }
                cout << endl;
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