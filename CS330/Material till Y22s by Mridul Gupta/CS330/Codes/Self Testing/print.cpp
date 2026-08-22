#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

void countAllTopologicalSorts(unordered_map<char, vector<char>>& graph, unordered_map<char, int>& inDegree, unordered_set<char>& visited, int& count, int totalNodes) {
    bool allVisited = true;

    for (const auto& node : graph) {
        char current = node.first;
        if (inDegree[current] == 0 && visited.find(current) == visited.end()) {
            allVisited = false;
            visited.insert(current);
            for (char neighbor : graph[current]) {
                inDegree[neighbor]--;
            }

            countAllTopologicalSorts(graph, inDegree, visited, count, totalNodes);

            // Backtrack
            visited.erase(current);
            for (char neighbor : graph[current]) {
                inDegree[neighbor]++;
            }
        }
    }

    if (allVisited && visited.size() == totalNodes) {
        count++;
    }
}

int ArrayChallenge(string strArr[], int arrLength) {
    unordered_map<char, vector<char>> graph;
    unordered_map<char, int> inDegree;
    unordered_set<char> uniqueNodes;

    // Build the graph
    for (int i = 0; i < arrLength; i++) {
        char u = strArr[i][0];
        char v = strArr[i][2];
        char relation = strArr[i][1];

        uniqueNodes.insert(u);
        uniqueNodes.insert(v);

        if (relation == '>') {
            graph[u].push_back(v);
            inDegree[v]++;
        } else {
            graph[v].push_back(u);
            inDegree[u]++;
        }
    }

    // Initialize inDegree for nodes with no incoming edges
    for (char node : uniqueNodes) {
        if (inDegree.find(node) == inDegree.end()) {
            inDegree[node] = 0;
        }
    }

    // Check for cycles (using BFS/DFS)
    queue<char> q;
    for (const auto& node : inDegree) {
        if (node.second == 0) {
            q.push(node.first);
        }
    }

    int visitedCount = 0;
    while (!q.empty()) {
        char current = q.front();
        q.pop();
        visitedCount++;
        for (char neighbor : graph[current]) {
            if (--inDegree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    if (visitedCount != uniqueNodes.size()) {
        return 0; // Cycle detected
    }

    // Count all topological sorts
    int count = 0;
    unordered_set<char> visited;
    countAllTopologicalSorts(graph, inDegree, visited, count, uniqueNodes.size());

    return count;
}

int main(void) {
    string A[] = {"A>B", "B<C", "C<D"};
    int arrLength = sizeof(A) / sizeof(A[0]);
    cout << ArrayChallenge(A, arrLength) << endl;
    return 0;
}
