// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0

#ifndef PokemonTSP_H
#define PokemonTSP_H

#include <vector>
#include <string> 
#include <cstdint> 
#include <iostream> 
#include <cmath> // sqrt(), pow()
#include <algorithm> // std::swap()
#include <limits> // numeric_limits<double>::infinity()
using std::cout;
using std::cin;


//-----------------------------------------------------------------------------------------//
//---------------------------------------------STRUCTS------------------------------------//
//---------------------------------------------------------------------------------------//


// new Vertex struct because location isn't important for B & C
struct VertexTSP {
    int x;
    int y;
    VertexTSP(int x1, int y1): x(x1), y(y1) {}
};


//-----------------------------------------------------------------------------------------//
//-------------------------------------PokemonTSP Class Def-------------------------------//
//---------------------------------------------------------------------------------------//


class PokemonTSP {
public:
    PokemonTSP(const std::string& mode_in, const uint32_t nV); 
    void readInput(); 

private:
    // Data structures
    std::vector<VertexTSP> vertices;
    std::string mode;
    std::vector<std::vector<double>> distance_tracker; // For storing minimal edges for each node

    // For storing the best tour
    std::vector<uint32_t> bestTour;
    double bestTourLength; // initialized to infinity
    double currentLength = 0.0;
    uint32_t numVertices;

    // Helper functions
    double calculateDistance(const VertexTSP& v1, const VertexTSP& v2); 
    void genPerms(size_t permLength); 
    bool promising(size_t permLength);
    void solveOptimalTSP(); 
    void solveFASTTSP(); 
    void printTour(); 

};

//-----------------------------------------------------------------------------------------//
//-------------------PokemonTSP Private Class Implementations-----------------------------//
//---------------------------------------------------------------------------------------//


PokemonTSP::PokemonTSP(const std::string& mode_in, uint32_t nV)
    : mode(mode_in), bestTourLength(std::numeric_limits<double>::infinity()) {
        numVertices = nV;
        vertices.reserve(numVertices);
} // ctor


void PokemonTSP::readInput() {

    // error checking for size constraints
    if (numVertices < 3) {
        std::cerr << "Invalid number of vertices. Must be at least 3.\n";
        exit(1);
    }

    // read in vertices and store in vertices vector
    for (uint32_t i = 0; i < numVertices; i++) {
        int x, y;
        cin >> x >> y;
        vertices.emplace_back(x, y); // puts into correct vertice number
    } // for

    // decide which mode to run and run
    if (mode == "FASTTSP") solveFASTTSP();
    if (mode == "OPTTSP") solveOptimalTSP();

    printTour();

} // readInput() --- driver


//-----------------------------------------------------------------------------------------//
//--------------------PokemonTSP Public Class Implementations-----------------------------//
//---------------------------------------------------------------------------------------//


double PokemonTSP::calculateDistance(const VertexTSP &v1, const VertexTSP &v2) {
    return sqrt(pow((v1.x - v2.x), 2) + pow((v1.y - v2.y), 2));
} // calcDist()


void PokemonTSP::genPerms(size_t permLength) {

    // complete path - base case
    if (permLength == numVertices) {
        double dist = currentLength + distance_tracker[bestTour[permLength - 1]][0];
        if (dist < bestTourLength) bestTourLength = dist;
        return;
    } // if

    // not promising
    if (!promising(permLength)) return;


    for (size_t i = permLength; i < numVertices; ++i) {
        std::swap(bestTour[permLength], bestTour[i]);

        // Update current length
        double addedLength = distance_tracker[bestTour[permLength - 1]][bestTour[permLength]];
        currentLength += addedLength;

        genPerms(permLength + 1); // Recursive call w/ permLength + 1

        // Backtrack
        currentLength -= addedLength;
        std::swap(bestTour[permLength], bestTour[i]);
    } // for

    return;
} // genPerms()


bool PokemonTSP::promising(size_t permLength) {
    // Nodes not yet in the path
    std::vector<uint32_t> unvisited(bestTour.begin() + (long)permLength, bestTour.end());

    if (unvisited.empty()) return true;

    // Compute minimal distance from start node (0) to unvisited nodes
    // also compute minimal distance from last node to unvisited nodes
    double minStart = std::numeric_limits<double>::infinity();
    uint32_t lastNode = bestTour[permLength - 1];
    double minEnd = std::numeric_limits<double>::infinity();


    for (uint32_t i : unvisited) {

        if (distance_tracker[0][i] < minStart)
            minStart = distance_tracker[0][i];

        if (distance_tracker[lastNode][i] < minEnd)
            minEnd = distance_tracker[lastNode][i];

    } // for - min dist start and end to unvisited nodes


    // Use Prim's algorithm
    double mstCost = 0.0;
    size_t n = unvisited.size();
    std::vector<double> minEdge(n, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(n, false);

    minEdge[0] = 0.0;

    for (uint32_t i = 0; i < n; ++i) {

        // Find the unvisited node with smallest minEdge
        double minVal = std::numeric_limits<double>::infinity();
        uint32_t startNode = 0;
        for (uint32_t j = 0; j < n; ++j) {
            if (!visited[j] && (minEdge[j] < minVal)) {
                minVal = minEdge[j];
                startNode = j;
            }
        } // for - find unvisited node with smallest minEdge

        // mark true and add
        visited[startNode] = true;
        mstCost += minEdge[startNode];

        // Update minEdge for the remaining nodes
        for (uint32_t j = 0; j < n; ++j) {

            if (!visited[j]) {
                double dist = distance_tracker[unvisited[startNode]][unvisited[j]];
                if (dist < minEdge[j]) {
                    minEdge[j] = dist;
                }
            }
        } // for

    } // for - find MST

    return (currentLength + minStart + minEnd + mstCost) < bestTourLength;
} // promising()



void PokemonTSP::solveOptimalTSP() {
    // Initialize distance tracker
    distance_tracker.resize(numVertices, std::vector<double>(numVertices)); // resize
    for (size_t i = 0; i < numVertices; ++i) { // and fill
        for (size_t j = 0; j < numVertices; ++j) {
            distance_tracker[i][j] = calculateDistance(vertices[i], vertices[j]);
        }
    }

    solveFASTTSP(); // Start with FASTTSP solution
    genPerms(1); // generate all permutations starting from index 1

    return;
} // solveOptimalTSP()


void PokemonTSP::solveFASTTSP() {
    
    double tour_distance = 0.0;
    std::vector<uint32_t> path;
    path.reserve(numVertices);
    std::vector<bool> visited(numVertices, false);
    

    uint32_t startNode = 0; // Start at node 0
    visited[startNode] = true; // mark true
    path.push_back(startNode); // push back

    double minNext = std::numeric_limits<double>::infinity();

    // find closest node to start with simple loop
    for (uint32_t i = 1; i < numVertices; i++) {
        double distance = calculateDistance(vertices[0], vertices[i]);

        // if distance is less than minNext, update
        if (distance < minNext) {
            startNode = i;
            minNext = distance;
        }

    } // for - find closest node to start


    // mark startNode (now node closest to 0) as visited and push
    visited[startNode] = true;
    path.push_back(startNode);

    path.push_back(0);
    tour_distance += minNext * 2;


    // process the rest of the nodes except for first two
    for (uint32_t i = 2; i < numVertices; i++) {

        // find next node to insert
        uint32_t next = 0;
        while (visited[next]) {
            next++;

            // just in case
            if (next == numVertices) break; // delete if exceeds time
        } // while - finds next node to insert

        // mark as visited and grab vertex
        visited[next] = true;
        VertexTSP insert = vertices[next];

        // check shortest edge to insert
        double minInsert = std::numeric_limits<double>::infinity();
        long id = 0;

        // find best edge to insert into by comparing all edges
        for (uint32_t j = 0; j < path.size() - 1; j++) {
            // edge and next
            VertexTSP edge1 = vertices[path[j]];
            VertexTSP edge2 = vertices[path[j+1]];

            // find distances
            double edge1_insert = calculateDistance(edge1, insert);
            double insert_edge2 = calculateDistance(insert, edge2);
            double before = calculateDistance(edge1, edge2);

            // if the new distance is less than the minInsert, update
            if ((edge1_insert + insert_edge2 - before) < minInsert) {
                id = j + 1; // insert after edge1
                // update minInsert with new bound
                minInsert = edge1_insert + insert_edge2 - before;
            }
        } // for - find best edge to insert into

        // add distance to tour and insert into path at specific index
        tour_distance += minInsert;
        path.insert(path.begin() + id, next);

    } // for - process the rest of the nodes

    // copy over and remove last node (0)
    bestTour = path;  
    bestTourLength = tour_distance; 
    path.pop_back();
      
    return;
} // solveFASTTSP()


void PokemonTSP::printTour() {
    cout << bestTourLength << "\n";

    // Do not include the last node (return to start) in the output
    for (size_t i = 0; i < bestTour.size() - 1; ++i) {
        cout << bestTour[i] << " "; // output the vertex index followed by a space
    }
    cout << "\n"; // Ensure there's a newline at the end
} // printTour()


#endif // PokemonTSP
