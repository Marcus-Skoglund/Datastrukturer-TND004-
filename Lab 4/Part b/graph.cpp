/*********************************************
 * file:	~\code4b\graph.h                  *
 * remark:implementation of undirected graphs *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <fmt/core.h>
#include <cassert>     // assert
#include <limits>      // std::numeric_limits
#include <algorithm>   // std::make_heap(), std::pop_heap(), std::push_heap()

#include "graph.h"
#include "dsets.h"

// Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

// -- CONSTRUCTORS

// Create a graph with n vertices and no vertices
Graph::Graph(int n) : table(n + 1), size{n}, n_edges{0} {
    assert(n >= 1);
}

Graph::Graph(const std::vector<Edge> &V, int n) : Graph{n} {
    for (auto e : V) {
        insertEdge(e);
    }
}

// -- MEMBER FUNCTIONS

// insert undirected edge e
// update weight if edge e is present
void Graph::insertEdge(const Edge &e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto edge_insertion = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        if (auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
                                   [e1](const Edge &ed) { return e1.links_same_nodes(ed); });
            it == end(T[e1.from])) {
            T[e1.from].push_back(e1);  // insert new edge e1
            ++n;                       // increment the counter of edges
        } else {
            it->weight = e1.weight;  // update the weight
        }
    };

    edge_insertion(e);
    edge_insertion(e.reverse());
}

// remove undirected edge e
void Graph::removeEdge(const Edge &e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto edgeRemoval = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
                               [e1](const Edge &ed) { return e1.links_same_nodes(ed); });

        assert(it != end(T[e1.from]));
        T[e1.from].erase(it);  // remove edge e1
        --n;                   // decrement the counter of edges
    };

    edgeRemoval(e);
    edgeRemoval(e.reverse());
}

// Prim's minimum spanning tree algorithm
void Graph::mstPrim() const {
    std::vector<int> dist(size + 1, std::numeric_limits<int>::max());
    std::vector<int> path(size + 1, 0);
    std::vector<bool> done(size + 1, false);

    // *** TODO ***
    
    int inf = std::numeric_limits<int>::max();
    
    for(int i = 1; i <= size; ++i){
        dist[i] = inf;
        path[i] = 0;
        done[i] = false;
    }
    
    int v = 1;  // start from any given vertex v
    dist[v] = 0; // current distance we're tracking (influenced by the weights)
    done[v] = true; // bool to not visit node again
    int totalWeight = 0;

    while(true){
        
        for(const auto& e : table[v]){ // edges from each vertex v
            
            int u = e.to;
            if(done[u] == false && dist[u] > e.weight){
                // checks already known distance against new parents distance + weights
                path[u] = v; // update parent node
                dist[u] = e.weight;
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
        done[v] = true;
        
        Edge edge_disp = Edge(path[v], v, min_dist);
        std::cout << edge_disp << std::endl;
        
        totalWeight += min_dist;
    }
    std::cout << std::endl << "total weight: " << totalWeight << std::endl;
}


// Kruskal's minimum spanning tree algorithm
void Graph::mstKruskal() const {
    // *** TODO ***
    
    std::vector<Edge> edges;
    edges.reserve(size - 1);
    
    for(int i = 1; i <= size; ++i){ // we create the graph with all edges to store in a vector
        for(const auto& e : table[i]){
            if (e.from < e.to) { // no duplicates (a->b, no b->a)
                edges.push_back(e);
            }
        }
    }
    
    DSets D(size);
    std::make_heap(edges.begin(), edges.end(), std::greater<Edge>{}); // make min-heap with path weights
    
    int counter = 0;
    int totalWeight = 0;
    
    while(counter  < (size - 1)){ // paths = vertices - 1
        // moves root to last and maintains min-heap with percolate
        std::pop_heap(edges.begin(), edges.end(), std::greater<Edge>{});
        // make a copy of the root which is now last in the min-heap, we proceed with this edge in the algorithm
        Edge e = edges.back();
        // we remove the edge from the heap/vector
        edges.pop_back();
        
        int root1 = D.find(e.from);
        int root2 = D.find(e.to);
        
        if(root1 != root2){ // if roots are the same skip
            
            std::cout << e << std::endl;
            totalWeight += e.weight;
            D.join(root1, root2);
            
            
            counter++;
        }
    }
    std::cout << "\nTotal weight: " << totalWeight << std::endl;
}

// print graph
void Graph::printGraph() const {
    std::cout << fmt::format("{:-<66}\n", '-');
    std::cout << "Vertex  adjacency lists\n";
    std::cout << fmt::format("{:-<66}\n", '-');

    for (int v = 1; v <= size; v++) {
        std::cout << fmt::format("{:4} : ", v);
        for (auto const &e : table[v]) {
            std::cout << fmt::format("({:2}, {:2}) ", e.to, e.weight);
        }
        std::cout << "\n";
    }
    std::cout << fmt::format("{:-<66}\n", '-');
}
