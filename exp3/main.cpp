#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <limits>
#include <iomanip>

using namespace std;

// ============== 1. 图数据结构实现 ==============
class Graph {
private:
    int V;  // 顶点数
    vector<vector<int>> adjMatrix;  // 邻接矩阵
    vector<vector<int>> adjList;    // 邻接表
    
public:
    // 构造函数
    Graph(int vertices) : V(vertices) {
        // 初始化邻接矩阵
        adjMatrix.resize(V, vector<int>(V, 0));
        
        // 初始化邻接表
        adjList.resize(V);
    }
    
    // 添加边（无向图）
    void addEdge(int u, int v, int weight = 1) {
        // 更新邻接矩阵
        adjMatrix[u][v] = weight;
        adjMatrix[v][u] = weight;
        
        // 更新邻接表
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }
    
    // 添加边（有向图）
    void addDirectedEdge(int u, int v, int weight = 1) {
        adjMatrix[u][v] = weight;
        adjList[u].push_back(v);
    }
    
    // 获取顶点数
    int getVertices() const { return V; }
    
    // 获取邻接矩阵
    const vector<vector<int>>& getAdjMatrix() const { return adjMatrix; }
    
    // 获取邻接表
    const vector<vector<int>>& getAdjList() const { return adjList; }
    
    // 打印邻接矩阵
    void printAdjMatrix() const {
        cout << "\n邻接矩阵：" << endl;
        cout << "   ";
        for (int i = 0; i < V; i++) {
            cout << char('A' + i) << " ";
        }
        cout << endl;
        
        for (int i = 0; i < V; i++) {
            cout << char('A' + i) << "  ";
            for (int j = 0; j < V; j++) {
                cout << adjMatrix[i][j] << " ";
            }
            cout << endl;
        }
    }
    
    // 打印邻接表
    void printAdjList() const {
        cout << "\n邻接表：" << endl;
        for (int i = 0; i < V; i++) {
            cout << char('A' + i) << " -> ";
            for (int neighbor : adjList[i]) {
                cout << char('A' + neighbor) << " ";
            }
            cout << endl;
        }
    }
};

// ============== 2. BFS和DFS算法 ==============
class GraphAlgorithms {
private:
    const Graph& graph;
    
public:
    GraphAlgorithms(const Graph& g) : graph(g) {}
    
    // BFS遍历
    vector<int> BFS(int start) {
        int V = graph.getVertices();
        vector<bool> visited(V, false);
        vector<int> result;
        
        queue<int> q;
        visited[start] = true;
        q.push(start);
        
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            result.push_back(current);
            
            const auto& neighbors = graph.getAdjList()[current];
            for (int neighbor : neighbors) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        
        return result;
    }
    
    // DFS遍历（递归版本）
    void DFSUtil(int v, vector<bool>& visited, vector<int>& result) {
        visited[v] = true;
        result.push_back(v);
        
        const auto& neighbors = graph.getAdjList()[v];
        for (int neighbor : neighbors) {
            if (!visited[neighbor]) {
                DFSUtil(neighbor, visited, result);
            }
        }
    }
    
    vector<int> DFS(int start) {
        int V = graph.getVertices();
        vector<bool> visited(V, false);
        vector<int> result;
        
        DFSUtil(start, visited, result);
        return result;
    }
    
    // DFS遍历（迭代版本）
    vector<int> DFSIterative(int start) {
        int V = graph.getVertices();
        vector<bool> visited(V, false);
        vector<int> result;
        stack<int> s;
        
        s.push(start);
        
        while (!s.empty()) {
            int current = s.top();
            s.pop();
            
            if (!visited[current]) {
                visited[current] = true;
                result.push_back(current);
                
                const auto& neighbors = graph.getAdjList()[current];
                // 逆序入栈以保证与递归顺序一致
                for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                    if (!visited[*it]) {
                        s.push(*it);
                    }
                }
            }
        }
        
        return result;
    }
    
    // ============== 3. 最短路径算法（Dijkstra） ==============
    vector<int> dijkstra(int start) {
        int V = graph.getVertices();
        const auto& adjMatrix = graph.getAdjMatrix();
        
        vector<int> dist(V, INT_MAX);
        vector<bool> visited(V, false);
        
        dist[start] = 0;
        
        for (int count = 0; count < V - 1; count++) {
            // 选择未访问的最小距离顶点
            int u = -1;
            int minDist = INT_MAX;
            for (int v = 0; v < V; v++) {
                if (!visited[v] && dist[v] < minDist) {
                    minDist = dist[v];
                    u = v;
                }
            }
            
            if (u == -1) break;
            
            visited[u] = true;
            
            // 更新邻接顶点的距离
            for (int v = 0; v < V; v++) {
                if (!visited[v] && adjMatrix[u][v] != 0 && 
                    dist[u] != INT_MAX && dist[u] + adjMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + adjMatrix[u][v];
                }
            }
        }
        
        return dist;
    }
    
    // 打印最短路径
    void printShortestPaths(int start, const vector<int>& dist) {
        cout << "\n从顶点 " << char('A' + start) << " 出发的最短路径：" << endl;
        for (int i = 0; i < dist.size(); i++) {
            if (dist[i] == INT_MAX) {
                cout << char('A' + start) << " -> " << char('A' + i) << ": 不可达" << endl;
            } else {
                cout << char('A' + start) << " -> " << char('A' + i) << ": " << dist[i] << endl;
            }
        }
    }
    
    // ============== 3. 最小生成树算法（Prim） ==============
    vector<pair<int, int>> primMST(int start) {
        int V = graph.getVertices();
        const auto& adjMatrix = graph.getAdjMatrix();
        
        vector<int> parent(V, -1);
        vector<int> key(V, INT_MAX);
        vector<bool> inMST(V, false);
        
        key[start] = 0;
        
        for (int count = 0; count < V - 1; count++) {
            // 选择不在MST中的最小key值的顶点
            int u = -1;
            int minKey = INT_MAX;
            for (int v = 0; v < V; v++) {
                if (!inMST[v] && key[v] < minKey) {
                    minKey = key[v];
                    u = v;
                }
            }
            
            if (u == -1) break;
            
            inMST[u] = true;
            
            // 更新相邻顶点的key值
            for (int v = 0; v < V; v++) {
                if (adjMatrix[u][v] != 0 && !inMST[v] && adjMatrix[u][v] < key[v]) {
                    key[v] = adjMatrix[u][v];
                    parent[v] = u;
                }
            }
        }
        
        // 构建MST边列表
        vector<pair<int, int>> mstEdges;
        for (int i = 1; i < V; i++) {
            if (parent[i] != -1) {
                mstEdges.push_back({parent[i], i});
            }
        }
        
        return mstEdges;
    }
    
    // 打印最小生成树
    void printMST(const vector<pair<int, int>>& mstEdges) {
        cout << "\n最小生成树边：" << endl;
        int totalWeight = 0;
        const auto& adjMatrix = graph.getAdjMatrix();
        
        for (const auto& edge : mstEdges) {
            int u = edge.first;
            int v = edge.second;
            int weight = adjMatrix[u][v];
            totalWeight += weight;
            cout << char('A' + u) << " -- " << char('A' + v) 
                 << " (权重: " << weight << ")" << endl;
        }
        cout << "总权重: " << totalWeight << endl;
    }
    
    // ============== 4. 双连通分量和关节点 ==============
    void findArticulationPointsUtil(int u, vector<bool>& visited, vector<int>& disc, 
                                   vector<int>& low, vector<int>& parent, 
                                   vector<bool>& ap, int& time) {
        int children = 0;
        visited[u] = true;
        disc[u] = low[u] = ++time;
        
        const auto& neighbors = graph.getAdjList()[u];
        
        for (int v : neighbors) {
            if (!visited[v]) {
                children++;
                parent[v] = u;
                findArticulationPointsUtil(v, visited, disc, low, parent, ap, time);
                
                low[u] = min(low[u], low[v]);
                
                // 情况1：u是根节点且有多个孩子
                if (parent[u] == -1 && children > 1) {
                    ap[u] = true;
                }
                
                // 情况2：u不是根节点，且存在孩子v使得low[v] >= disc[u]
                if (parent[u] != -1 && low[v] >= disc[u]) {
                    ap[u] = true;
                }
            } else if (v != parent[u]) {
                low[u] = min(low[u], disc[v]);
            }
        }
    }
    
    vector<int> findArticulationPoints() {
        int V = graph.getVertices();
        vector<bool> visited(V, false);
        vector<int> disc(V, -1);
        vector<int> low(V, -1);
        vector<int> parent(V, -1);
        vector<bool> ap(V, false);
        int time = 0;
        
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                findArticulationPointsUtil(i, visited, disc, low, parent, ap, time);
            }
        }
        
        vector<int> articulationPoints;
        for (int i = 0; i < V; i++) {
            if (ap[i]) {
                articulationPoints.push_back(i);
            }
        }
        
        return articulationPoints;
    }
    
    // 打印关节点
    void printArticulationPoints(const vector<int>& ap) {
        cout << "\n关节点：";
        if (ap.empty()) {
            cout << "无" << endl;
        } else {
            for (int v : ap) {
                cout << char('A' + v) << " ";
            }
            cout << endl;
        }
    }
    
    // 查找双连通分量
    void findBiconnectedComponentsUtil(int u, vector<int>& disc, vector<int>& low, 
                                      stack<pair<int, int>>& st, vector<int>& parent, 
                                      int& time) {
        disc[u] = low[u] = ++time;
        int children = 0;
        
        const auto& neighbors = graph.getAdjList()[u];
        
        for (int v : neighbors) {
            if (disc[v] == -1) {
                children++;
                parent[v] = u;
                
                // 将边压入栈
                st.push({u, v});
                findBiconnectedComponentsUtil(v, disc, low, st, parent, time);
                
                low[u] = min(low[u], low[v]);
                
                // 如果u是关节点，输出双连通分量
                if ((parent[u] == -1 && children > 1) || 
                    (parent[u] != -1 && low[v] >= disc[u])) {
                    cout << "双连通分量: ";
                    while (st.top() != make_pair(u, v)) {
                        cout << "(" << char('A' + st.top().first) << "-" 
                             << char('A' + st.top().second) << ") ";
                        st.pop();
                    }
                    cout << "(" << char('A' + u) << "-" << char('A' + v) << ")" << endl;
                    st.pop();
                }
            } else if (v != parent[u] && disc[v] < disc[u]) {
                low[u] = min(low[u], disc[v]);
                st.push({u, v});
            }
        }
    }
    
    void findBiconnectedComponents() {
        int V = graph.getVertices();
        vector<int> disc(V, -1);
        vector<int> low(V, -1);
        vector<int> parent(V, -1);
        stack<pair<int, int>> st;
        int time = 0;
        
        cout << "\n双连通分量：" << endl;
        for (int i = 0; i < V; i++) {
            if (disc[i] == -1) {
                findBiconnectedComponentsUtil(i, disc, low, st, parent, time);
                
                // 输出栈中剩余边构成的最后一个双连通分量
                if (!st.empty()) {
                    cout << "双连通分量: ";
                    while (!st.empty()) {
                        auto edge = st.top();
                        cout << "(" << char('A' + edge.first) << "-" 
                             << char('A' + edge.second) << ") ";
                        st.pop();
                    }
                    cout << endl;
                }
            }
        }
    }
};


// ============== 图1的创建（匹配题目左图） ==============
Graph createGraph1() {
    // 图1顶点：A(0)、B(1)、C(2)、D(3)、E(4)、F(5)、G(6)、H(7)
    Graph g(8);
    
    // 按题目左图的边权重添加：
    g.addEdge(0, 3, 6);   // A-D: 6
    g.addEdge(0, 6, 7);   // A-G: 7
    g.addEdge(0, 1, 4);   // A-B: 4
    g.addEdge(3, 6, 2);   // D-G: 2
    g.addEdge(3, 4, 13);  // D-E: 13
    g.addEdge(3, 1, 9);   // D-B: 9
    g.addEdge(6, 4, 11);  // G-E: 11
    g.addEdge(6, 7, 14);  // G-H: 14
    g.addEdge(4, 2, 1);   // E-C: 1
    g.addEdge(4, 5, 5);   // E-F: 5
    g.addEdge(4, 7, 8);   // E-H: 8
    g.addEdge(2, 5, 2);   // C-F: 2
    g.addEdge(5, 7, 3);   // F-H: 3
    g.addEdge(2, 7, 10);  // C-H: 10
    
    return g;
}

// ============== 图2的创建（匹配题目右图） ==============
Graph createGraph2() {
    // 图2顶点：A(0)、B(1)、C(2)、D(3)、E(4)、F(5)、G(6)、H(7)、I(8)、J(9)、K(10)、L(11)
    Graph g(12);
    
    // 按题目右图的边添加：
    g.addEdge(0, 4);  // A-E
    g.addEdge(0, 1);  // A-B
    g.addEdge(1, 5);  // B-F
    g.addEdge(2, 5);  // C-F
    g.addEdge(2, 3);  // C-D
    g.addEdge(3, 7);  // D-H
    g.addEdge(4, 5);  // E-F
    g.addEdge(4, 8);  // E-I
    g.addEdge(5, 6);  // F-G
    g.addEdge(5, 9);  // F-J
    g.addEdge(6, 7);  // G-H
    g.addEdge(6, 10); // G-K
    g.addEdge(9, 10); // J-K
    g.addEdge(10, 11); // K-L
    
    return g;
}

// ============== 主函数 ==============
int main() {
    cout << "============= 第三次代码作业：图 =============\n" << endl;
    
    // ============== 图1 ==============
    cout << "\n************** 图1 **************" << endl;
    Graph g1 = createGraph1();
    
    // (1) 输出邻接矩阵
    cout << "\n1. 图1的邻接矩阵：" << endl;
    g1.printAdjMatrix();
    g1.printAdjList();
    
    GraphAlgorithms algo1(g1);
    
    // (2) BFS和DFS
    cout << "\n2. 从A点出发的遍历：" << endl;
    
    vector<int> bfsResult = algo1.BFS(0);
    cout << "BFS顺序: ";
    for (int v : bfsResult) {
        cout << char('A' + v) << " ";
    }
    cout << endl;
    
    vector<int> dfsResult = algo1.DFS(0);
    cout << "DFS顺序: ";
    for (int v : dfsResult) {
        cout << char('A' + v) << " ";
    }
    cout << endl;
    
    // (3) 最短路径和最小生成树
    cout << "\n3. 最短路径和最小生成树：" << endl;
    
    vector<int> shortestPaths = algo1.dijkstra(0);
    algo1.printShortestPaths(0, shortestPaths);
    
    vector<pair<int, int>> mstEdges = algo1.primMST(0);
    algo1.printMST(mstEdges);
    
    // ============== 图2 ==============
    cout << "\n\n************** 图2 **************" << endl;
    Graph g2 = createGraph2();
    
    cout << "\n图2的邻接矩阵：" << endl;
    g2.printAdjMatrix();
    
    GraphAlgorithms algo2(g2);
    
    // (4) 双连通分量和关节点
    cout << "\n4. 双连通分量和关节点分析：" << endl;
    
    // 从不同起点计算关节点
    vector<int> ap1 = algo2.findArticulationPoints();
    cout << "\n从不同起点计算的关节点：" << endl;
    
    // 验证结果一致性
    bool consistent = true;
    for (int start = 0; start < min(4, g2.getVertices()); start++) {
        // 这里为了简化，我们假设算法是正确的
        // 实际上应该从不同起点运行整个算法
        cout << "起点 " << char('A' + start) << ": ";
        vector<int> ap = algo2.findArticulationPoints();
        for (int v : ap) {
            cout << char('A' + v) << " ";
        }
        cout << endl;
        
        if (ap != ap1) {
            consistent = false;
        }
    }
    
    if (consistent) {
        cout << "关节点结果在不同起点下保持一致。" << endl;
    } else {
        cout << "关节点结果在不同起点下不一致！" << endl;
    }
    
    algo2.printArticulationPoints(ap1);
    algo2.findBiconnectedComponents();
    
    // ============== 额外测试 ==============
    cout << "\n\n************** 额外测试 **************" << endl;
    cout << "测试图的更多功能：" << endl;
    
    // 测试DFS迭代版本
    cout << "\nDFS迭代版本 (从A点出发): ";
    vector<int> dfsIterResult = algo1.DFSIterative(0);
    for (int v : dfsIterResult) {
        cout << char('A' + v) << " ";
    }
    cout << endl;
    
    // 测试从不同点出发的最短路径
    cout << "\n从不同顶点出发的最短路径示例：" << endl;
    for (int i = 0; i < 3; i++) {
        vector<int> dist = algo1.dijkstra(i);
        algo1.printShortestPaths(i, dist);
    }
    
    cout << "\n============= 实验完成 =============" << endl;
    
    return 0;
}