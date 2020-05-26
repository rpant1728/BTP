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
#include <sstream> 

using namespace std;

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

        Node(int _i1,int _j1,int _i2,int _j2,int _k, bool _s){
            i1 = _i1;
            i2 = _i2;
            j1 = _j1;
            j2 = _j2;
            k = _k;
            if(_k%2 == 0) turn = true;
            else turn = false;
            isSafe = _s;
            isWinning = false;
            id = to_string(i1)+"~"+to_string(j1)+"~"+to_string(i2)+"~"+to_string(j2)+"~"+to_string(_k);
        }
};

bool isValid(int i1, int j1, int i2, int j2, int m, int n){
    if(i1 >= 0 && j1 >= 0 && i2 >= 0 && j2 >=0 && i1 < m && j1 < n && i2 < m && j2 < n){
        return true;
    }
    return false;
}

class Game{
    public:
        int m, n;
        unordered_map <string, Node*> nodeMap;
        unordered_map <string, list <string> > adj;

        unordered_map <string, list <string> > predecessorList; 
        unordered_map <string, int> successorCount;

        unordered_set <string> notWinningSet;
        unordered_set <string> winningSet;
        unordered_set <string> player1Nodes;
        unordered_set <string> safeSet;
        unordered_set <string> unSafeSet;

        unordered_map <string, PW> pws;
        unordered_map <string, PW> minPWs;

        unordered_map<int,unordered_map<int,int>> getMinSteps;

        unordered_set <string> explored;
        bool optimization;

        Game(bool opt, int _m , int _n){
            m = _m;
            n = _n;
            optimization = opt;
        }

        void setNode(int _i1, int _j1, int _i2, int _j2, int _k, bool _s){
            string id=to_string(_i1)+"~"+to_string(_j1)+"~"+to_string(_i2)+"~"+to_string(_j2)+"~"+to_string(_k);
            if(isValid(_i1, _j1, _i2, _j2, m, n)){
                nodeMap[id] = new Node(_i1,_j1,_i2,_j2,_k, _s);
                // cout << nodeMap[id]->id << " added!" << endl;
                if(_k%2 == 0){
                    player1Nodes.insert(id);
                }
                adj[id]=list<string>();
                predecessorList[id]=list<string>();
            }
        }

        void addMove(string u, string v){
            if(nodeMap.find(u)!=nodeMap.end() && nodeMap.find(u)!=nodeMap.end()){
                adj[u].push_back(v);
            }
        }

        void reverseBFS(string v, int k){
            predecessorList.clear();
            successorCount.clear();
            for(auto i:adj){
                for(string j: i.second){
                    predecessorList[j].push_back(i.first);
                    // successorCount[i.first]++;
                }
            }
            propagateBFS(v, 0, k);
        }


        void propagateBFS(string v, int i, int k){
            if(safeSet.find(v) != safeSet.end() || i > 2*k){
                return;
            }
            Node *node = nodeMap[v];
            if(node->k+i <= 6){
                safeSet.insert(v);
                nodeMap[v]->isSafe=true;
            }
            // getMinSteps[node->i1][node->j1]=i;
            for(string u: predecessorList[v]){
                // successorCount[u]--;
                // if(player1Nodes.find(u) == player1Nodes.end() || successorCount[u] == 0){
                propagateBFS(u,i+1, k);
                // }
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
                if(notWinningSet.find(i.first)==notWinningSet.end() && safeSet.find(i.first) != safeSet.end()){
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

        void exploreAllPathsUtil(string u, string start, Node *node, unordered_set <string> visited, list <string> &currentPlayer2Nodes, string successor, list <string> &path){
            if(visited.find(u) != visited.end()) return;
            visited.insert(u);
            // cout << "Path: ";
            // for(string i: path){
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
                if(unSafeSet.find(v) != unSafeSet.end()){
                    continue;
                }
                else if(winningSet.find(v) != winningSet.end()){
                    cout << v << endl;
                    Node* node1 = nodeMap[v];
                    string s = "";
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
                    cout << v << " ";
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
                            // cout << "hjk" << endl;
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

void printSet(unordered_set <string> s){
        for(string i: s){
        cout << i << " ";
    }  
    cout << endl;
}



int main(int argc, char *argv[]){
    clock_t startTime = clock();
    bool optimization = false;
    if(argc == 2) optimization = true;

    int m, n, k, p;
    cin >> m >> n >> k >> p;

    int x1, y1, x2, y2, x3, y3;
    cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;

    unordered_set <string> obstacles;

    for(int i=0; i<p; i++){
        int x, y;
        cin >> x >> y;
        obstacles.insert(to_string(x) + "~" + to_string(y));
    }
    
    Game game(optimization, m, n);

    for(int l=0; l<2*k; l++){
        for(int i1=0; i1<m; i1++){
            for(int j1=0; j1<n; j1++){
                for(int i2=0; i2<m; i2++){
                    for(int j2=0; j2<n; j2++){
                        vector <pair <int, int>> possible{{0,1}, {1,0}, {-1,0}, {0,-1}};
                        string currPlayer1 = to_string(i1) + "~" + to_string(j1);
                        string currPlayer2 = to_string(i2) + "~" + to_string(j2);
                        if(obstacles.find(currPlayer1) != obstacles.end() || obstacles.find(currPlayer2) != obstacles.end()){
                            continue;
                        }
                        string currState1 = currPlayer1 + "~" + currPlayer2 + "~" + to_string(l);
                        string currState2 = currPlayer1 + "~" + currPlayer2 + "~" + to_string(l);
                        if(!(i1 == i2 && j1 == j2)){ 
                            if(game.nodeMap.find(currState1) == game.nodeMap.end()){
                                game.setNode(i1,j1,i2,j2,l,true);
                            }
                            if(game.nodeMap.find(currState2) == game.nodeMap.end()){
                                game.setNode(i1,j1,i2,j2,l,true);
                            }
                            if(i1==x3 && j1==y3){
                                game.winningSet.insert(currState1);
                                game.winningSet.insert(currState2);
                                game.addMove(currState1, currState1);
                                game.addMove(currState2, currState2);
                            }
                            else{
                                if(l%2 == 0){
                                    for(int o=0;o<4; o++){
                                        string nextPlayer1 = to_string(i1+possible[o].first) + "~" + to_string(j1+possible[o].second);
                                        if(obstacles.find(nextPlayer1) == obstacles.end() && isValid(i1+possible[o].first,j1+possible[o].second,i2,j2,m,n)){
                                            string nextState1 = nextPlayer1+"~"+currPlayer2+"~"+to_string(l+1);
                                            if(game.nodeMap.find(nextState1) == game.nodeMap.end()){
                                                game.setNode(i1+possible[o].first,j1+possible[o].second,i2,j2,l+1,true);
                                            }
                                            game.addMove(currState1, nextState1);
                                        }
                                    }
                                }
                                else{
                                    for(int o=0;o<4; o++){
                                        string nextPlayer2 = to_string(i2+possible[o].first) + "~" + to_string(j2+possible[o].second);
                                        if(obstacles.find(nextPlayer2) == obstacles.end() && isValid(i1,j1,i2+possible[o].first,j2+possible[o].second,m,n)){
                                            string nextState2 = currPlayer1+"~"+nextPlayer2+"~"+to_string(l+1);
                                            if(game.nodeMap.find(nextState2) == game.nodeMap.end()  && isValid(i1,j1,i2,j2,m,n)){
                                                game.setNode(i1,j1,i2+possible[o].first,j2+possible[o].second,l+1,true);
                                            }
                                            game.addMove(currState2, nextState2);
                                        }
                                    }
                                }
                            }
                        }
                        else{
                            if(game.nodeMap.find(currState1) == game.nodeMap.end()){
                                game.setNode(i1,j1,i2,j2,l+1,false);
                            }
                            if(game.nodeMap.find(currState2) == game.nodeMap.end()){
                                game.setNode(i1,j1,i2,j2,l+1,false);
                            }
                            game.unSafeSet.insert(currState1);
                            game.unSafeSet.insert(currState2);
                        }
                    }
                }
            }
        }
    }

    for(auto i: game.winningSet){
        game.reverseBFS(i, k);
    }

    game.initializePropagate();
    game.findWinningSet();

    printSet(game.winningSet);

    cout << game.nodeMap.size() << " " << game.winningSet.size() << endl;
    
    string startState = to_string(x1)+"~"+to_string(y1)+"~"+to_string(x2)+"~"+to_string(y2)+"~0";
    if(game.winningSet.find(startState) != game.winningSet.end()){
        cout << "Player 1 has a definite Winning Strategy!" << endl;
        return 0;
    }

    Node *node = game.nodeMap[startState];

    if(node->isSafe){
        game.exploreAllPaths(startState);
        game.getMinimalPWs(startState);
    } 

    cout << "Minimal Potential Winning Sets" << endl;
    if (game.minPWs.size() != 0){
        for(auto x: game.minPWs) {
            // cout << "Vertex " << x.first << endl;
            if(x.second.isCycle) cout << "Cycle: ";
            else cout << "Lasso: ";
            if(x.second.nodes.size() != 0){
                for(string k: x.second.nodes){
                    cout << k << " ";
                }
                cout << ": ";
            }
            if(x.second.path.size() != 0){
                for(string k: x.second.path){
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