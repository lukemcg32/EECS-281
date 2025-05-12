// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#ifndef PokemonMST_H
#define PokemonMST_H

#include <iostream>
#include <limits> // numeric_limits<double>::infinity()
#include <queue> // for PQ
#include <vector>
#include <cmath> // for sqrt() and pow(num, power)
using std::cout;
using std::cin;

//-----------------------------------------------------------------------------------------//
//---------------------------------------------STRUCTS------------------------------------//
//---------------------------------------------------------------------------------------//


struct Vertex {
    int x;
    int y;
    char location; // Land: l  -  Sea: s  -  Coast: c
    Vertex(int x1, int y1);
};


//-----------------------------------------------------------------------------------------//
//----------------------------------------Pokemon Class Def-------------------------------//
//---------------------------------------------------------------------------------------//

class PokemonMST {
public:
    PokemonMST(uint32_t nV);
    void readInput(); 

private:
    // data structures
    std::vector<Vertex> vertices;
    std::vector<std::pair<int, int>> edges;

    // member variables
    double totalWeight; // rounding handled in main.cpp
    uint32_t numVertices;

    // private member functions
    double calculateDistance(const Vertex& v1, const Vertex& v2);
    bool areConnected(const Vertex& v1, const Vertex& v2);
    bool findMST();
    void printMST();
};


//-----------------------------------------------------------------------------------------//
//-------------------------Class Implementations (Public)---------------------------------//
//---------------------------------------------------------------------------------------//


PokemonMST::PokemonMST(uint32_t nV) : totalWeight(0.0), numVertices(nV) {
    vertices.reserve(numVertices);
} // ctor

void PokemonMST::readInput() {

    // read input
    for (uint32_t i = 0; i < numVertices; i++) {
        int x, y;
        cin >> x >> y;
        vertices.emplace_back(x, y);
    } // for

    // reading done, now see if we can process and print
    if (!findMST()) {
        std::cerr << "Cannot construct MST\n";
        exit(1);
    }

    printMST();

    return;
} // readInput()


//-----------------------------------------------------------------------------------------//
//-------------------------Class Implementations (Public)---------------------------------//
//---------------------------------------------------------------------------------------//


bool PokemonMST::areConnected(const Vertex& v1, const Vertex& v2) {
    // if between land and sea, its false ... otherwise true
    if ((v1.location == 'l' && v2.location == 's') || (v1.location == 's' && v2.location == 'l')) return false;
    return true;
} // areConnected()



double PokemonMST::calculateDistance(const Vertex& v1, const Vertex& v2) {
    return sqrt(pow((v1.x - v2.x), 2) + pow((v1.y - v2.y), 2));
} // calculateDistance()


// updated ARRAY based prim's algorithm
bool PokemonMST::findMST() {
    if (numVertices < 2) return false;

    // Prim's algorithm data structures
    std::vector<bool> in_mst(numVertices, false);
    std::vector<double> key(numVertices, std::numeric_limits<double>::infinity());
    std::vector<uint32_t> parent(numVertices, UINT32_MAX);

    // Start from vertex 0
    key[0] = 0.0;

    for (uint32_t count = 0; count < numVertices; ++count) {
        // Pick the minimum key vertex not yet included in MST
        double min_key = std::numeric_limits<double>::infinity();
        uint32_t u = UINT32_MAX;

        for (uint32_t v = 0; v < numVertices; ++v) {
            // if not discovered and less than upper bound, update
            if (!in_mst[v] && key[v] < min_key) {
                min_key = key[v];
                u = v;
            }
        } // for

        // if u is UINT_MAX, graph is disconnected
        if (u == UINT32_MAX) return false;

        // marked vertex as visited
        in_mst[u] = true;

        // Update key and parent for adjacent vertices
        for (uint32_t v = 0; v < numVertices; ++v) {
            // if not discovered and connected, update
            if (!in_mst[v] && areConnected(vertices[u], vertices[v])) {
                double weight = calculateDistance(vertices[u], vertices[v]);
                if (weight < key[v]) {
                    key[v] = weight;
                    parent[v] = u;
                }
            }
        } // for
    }

    // Check if all vertices are connected
    for (bool connected : in_mst) {
        if (!connected) return false;
    }

    // Build the edges and calculate total weight
    for (uint32_t i = 1; i < numVertices; ++i) {
        if (parent[i] != UINT32_MAX) {
            uint32_t curr = i;
            uint32_t par = static_cast<uint32_t>(parent[i]);

            if (curr > par) std::swap(curr, par); // Ensure smaller index first

            edges.emplace_back(curr, par);
            totalWeight += calculateDistance(vertices[curr], vertices[par]);
        }
    } // for

    return true;
} // findMST()


void PokemonMST::printMST() {
    cout << totalWeight << std::endl;
    for (auto& edge : edges) {
        cout << edge.first << " " << edge.second << "\n";
    }

    return;
} // printMST()


//-----------------------------------------------------------------------------------------//
//-----------------------------STRUCTS Implementations------------------------------------//
//---------------------------------------------------------------------------------------//


Vertex::Vertex(int x1, int y1) : x(x1), y(y1) {
    if (x1 > 0 || y1 > 0) location = 'l'; // land
    else if (x1 < 0 && y1 < 0) location = 's'; // sea
    else location = 'c'; // coast
}

#endif // PokemonMST