/*********************************************
 * file:	~\code4a\digraph.cpp              *
 * remark: implementation of directed graphs  *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <limits>  //std::numeric_limits
#include <vector>
#include <cassert>
#include <queue>
#include <fmt/core.h>

#include "digraph.h"

// Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

// -- CONSTRUCTORS

Digraph::Digraph(int n)
    : table(n + 1)  // slot zero not used
    , size{n}       // number of verices
    , n_edges{0}
    , dist(n + 1)
    , path(n + 1)
    , done(n + 1) {
    assert(n >= 1);
    // Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero
}

// Create a digraph with n vertices and the edges in V
Digraph::Digraph(const std::vector<Edge>& V, int n) : Digraph{n} {
    for (auto e : V) {
        insertEdge(e);
    }
}

// -- MEMBER FUNCTIONS

// insert directed edge e = (u, v, w)
// update weight w if edge (u, v) is present
void Digraph::insertEdge(const Edge& e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    // Check if edge e already exists
    if (auto it = std::find_if(begin(table[e.from]), end(table[e.from]),
                               [e](const Edge& ed) { return e.links_same_nodes(ed); });
        it == end(table[e.from])) {
        table[e.from].push_back(e);  // insert new edge e
        ++n_edges;
    } else {
        it->weight = e.weight;  // update the weight
    }
}

// remove directed edge e
void Digraph::removeEdge(const Edge& e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto it = std::find_if(begin(table[e.from]), end(table[e.from]),
                           [e](const Edge& ed) { return e.links_same_nodes(ed); });

    assert(it != end(table[e.from]));
    table[e.from].erase(it);
    --n_edges;
}

// construct unweighted single source shortest path-tree for start vertex s
void Digraph::uwsssp(int s) const { // starting vertex, breadth-first search
    assert(s >= 1 && s <= size);

    // *** TODO ***
    // dist vector should be initialized with std::numeric_limits<int>::max()
    
    const int inf = std::numeric_limits<int>::max(); // so that we know if a isn't visited
    
    std::queue<int> Q;
        
    for (int i=1; i<= size; i++) {
        dist[i] = inf; // dist
        path[i] = 0; // path
    }
    
    dist[s] = 0; // current disatnce we're tracking (1 between all vertices)
    Q.push(s);
    
    while(!Q.empty()){ // queue for the nodess to visit
        int v = Q.front();
        Q.pop();
        
        for(const auto& e : table[v]){ // check all connected nodes
            int u = e.to;
            if(dist[u] == inf){ // if path already found for the vertex skip
                dist[u] = dist[v] + 1; // add 1 to path because all paths have 1 in distance
                path[u] = v;
                Q.push(u); // queue new nodes if they've not been visited
            }
        }
        //if this function finds e vertex in the graph with multiple edges connected it considers the first one it found and ignores the others, distance != inf
    }
    
}

// construct positive weighted single source shortest path-tree for start vertex s
// Dijktra’s algorithm
void Digraph::pwsssp(int s) const { // starting vertex
    assert(s >= 1 && s <= size);

    // *** TODO ***
    // dist vector should be initialized with std::numeric_limits<int>::max()
    
    const int inf = std::numeric_limits<int>::max(); // so that we know if a isn't visited
    
    // edge (u = from, v = to, weight)
    
    for (int i = 1 ; i<= size; ++i) {
           dist[i] = inf;
           path[i] = 0;
           done[i] = false;
       }
    dist[s] = 0; // current disatnce we're tracking (influenced by the weights)
    int v = s;
    done[s] = true; // bool to not visit node again
    
    while(true){
        
        for(const auto& e : table[v]){ // edges from each vertex v
            int u = e.to;
            if(done[u] == false && dist[u] > dist[v] + e.weight){
                // checks already known distance against new parents distance + weights
                dist[u] = dist[v] + e.weight; // adds cost from v + the weight
                path[u] = v; // update parent node
            }
        }
        
        int min_dist = inf;
        for(int i = 1; i <= size; ++i){ // loops throught every vertex
            if (!done[i] && dist[i] < min_dist){ // finds shortest distance
                min_dist = dist[i];
                v = i; // we walk to this vertex
            }
        }
        if (min_dist == inf){
            break;
        }
        done[v] = true; // mark as visited
    }
}

// print graph
void Digraph::printGraph() const {
    std::cout << fmt::format("{:-<66}\n", '-');
    std::cout << "Vertex  adjacency lists\n";
    std::cout << fmt::format("{:-<66}\n", '-');

    for (int v = 1; v <= size; ++v) {
        std::cout << fmt::format("{:4} : ", v);
        for (auto const& e : table[v]) {
            std::cout << fmt::format("({:2}, {:2}) ", e.to, e.weight);
        }
        std::cout << "\n";
    }
    std::cout << fmt::format("{:-<66}\n", '-');
}

// print shortest path tree for s
void Digraph::printTree() const {
    std::cout << fmt::format("{:-<22}\n", '-');
    // std::cout << "----------------------\n";
    std::cout << "vertex    dist    path\n";
    std::cout << fmt::format("{:-<22}\n", '-');
    // std::cout << "----------------------\n";

    for (int v = 1; v <= size; ++v) {
        std::cout << fmt::format("{:4} : {:6} {:6}\n", v,
                                 ((dist[v] == std::numeric_limits<int>::max()) ? -1 : dist[v]),
                                 path[v]);
    }
    std::cout << fmt::format("{:-<22}\n", '-');
    // std::cout << "----------------------\n";
}

// print shortest path from s to t and the corresponding path length
// Hint: consider using recursion
void Digraph::printPath(int t) const {
    assert(t >= 1 && t <= size);

    // *** TODO ***
    
    
    if(path[t] == 0){ // finds base case (root parent)
        std::cout << " " << t << " ";
        return;
    }
    
    printPath(path[t]); // drills down to root, then goes to next vertex until t, with print in evety step
    std::cout << " ->" << fmt::format("{:2}", t); // when parent is found, prints vertex and distance
    std::cout << fmt::format("({})", dist[t]);
}
