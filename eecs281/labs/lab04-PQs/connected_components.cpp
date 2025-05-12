/*  Identifier = 15C1680EE94C640EC35E1694295A3625C3254CBA

    EECS 281 Lab 4: Connected Components

    For this lab, you will write a program that calculates the
    number of connected components in an undirected graph.

    Your program will receive an input file in the following format:
    1) The first line of input contains the number of vertices V and number
       of edges E in the graph.
    2) The next E lines list out the connections between vertices in the
       graph in the format of "u v", which indicates the existence of an edge
       between u and v.

    For example, the following input file would be read as follows:
    4 2  (the graph has 4 vertices numbered from 0-3, and two edges exist in
          the graph)
    0 1  (the first of these edges connects vertex 0 with vertex 1)
    0 2  (the second of these edges connects vertex 0 with vertex 2)

    In this case, vertices {0, 1, 2} form a connected component, and vertex
    {3} forms a connected component (since 3 is not connected to anything).
    Thus, your program should print out 2.

    You will be using union-find to complete this lab. The following starter
    code has been provided. Feel free to modify this code in your
    implementation. Good luck!
*/

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include <vector>
using std::vector;


class Graph {
    uint32_t V;
    uint32_t E;
    vector<uint32_t> parent;
    vector<uint32_t> treeSize;
    uint32_t componentCount;

public:

    Graph(uint32_t v, uint32_t e)
        : V(v), E(e), parent(v), treeSize(v, 1), componentCount(v) {

        // so parents are always larger than kids
        for (uint32_t i = 0; i < V; ++i) {
            parent[i] = i;
        }

        // Read edges and perform union operations
        uint32_t u, w;
        for (uint32_t i = 0; i < E; ++i) {
            cin >> u >> w;
            union_set(u, w);
        }
    }  // Graph()


    uint32_t find_set(uint32_t v) {
        if (parent[v] != v) {
            parent[v] = find_set(parent[v]);  // Path compression
        }
        return parent[v];
    }  // find_set()


    void union_set(uint32_t a, uint32_t b) {
        uint32_t root_a = find_set(a);
        uint32_t root_b = find_set(b);

        if (root_a != root_b) {
            // Union by size
            if (treeSize[root_a] < treeSize[root_b]) {
                parent[root_a] = root_b;
                treeSize[root_b] += treeSize[root_a];
            } else {
                parent[root_b] = root_a;
                treeSize[root_a] += treeSize[root_b];
            }
            componentCount--;  // Decrease the number of components
        }
    }  // union_set()


    uint32_t count_components() {
        return componentCount;
    }  // count_components()
};  // Graph{}


int main() {
    // You do not need to modify main.
    std::ios_base::sync_with_stdio(false);
    uint32_t vertex_count, edge_count = 0;

    cin >> vertex_count;
    cin >> edge_count;

    Graph g(vertex_count, edge_count);

    cout << g.count_components() << endl;
    return 0;
}  // main()
