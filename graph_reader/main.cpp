#include <iostream>
#include "graph.h"
#include<vector>
#include <omp.h>

using namespace std;

uint32_t startNode = 3072626;
uint32_t endNode = 1;//last node in the graph
bool *checked;
bool notFound = true;

int64_t *depth;
int64_t *parents;

void clearDepth();

void clearParents();

void topDown_checkAndAdd(uint32_t i);

void bottomUp_checkAndAdd();

graph<long, long, int, long, long, char> *ginst;
uint32_t dist = 0;
uint32_t nextDist = 1;
uint32_t frontierSize, uncheckedVertices, unexploredEdges,frontierEdges;


int main(int args, char **argv) {
    std::cout << "Input: ./exe beg csr weight\n";
    if (args != 4) {
        std::cout << "Wrong input\n";
        return -1;
    }
    const char *beg_file = argv[1];
    const char *csr_file = argv[2];
    const char *weight_file = argv[3];
    ginst = new graph
            <long, long, int, long, long, char>
            (beg_file, csr_file, weight_file);


    depth = new int64_t[ginst->vert_count];
    parents = new int64_t[ginst->vert_count];
    clearParents();
    clearDepth();
    depth[startNode] = dist; //adding the start node to the frontier
    omp_set_num_threads(8);
    cout << "Starting now" << endl;
    cout << "Depth: " << depth[startNode] << "\tParents: " << parents[startNode] << endl;
    bool topDown = true;
    const int a = 14;
    const int b = 24;
    frontierSize,frontierEdges = 0;
    unexploredEdges = ginst->edge_count;
    uncheckedVertices = ginst->vert_count;
    while (notFound) {
//top-down approach
        if (topDown) { //TODO: This is inefficient because it scans through all nodes rather than just an adjacency list -- go back to the atomic add function in addition to an array to maximize efficiency in this state.
#pragma omp parallel for
            for (int i = 0; i < ginst->vert_count; i++) {
                if (depth[i] == dist) {
                    topDown_checkAndAdd(i);
                }
            }
        } else
            bottomUp_checkAndAdd();
        cout << dist << "\t" << (double) unexploredEdges/ (double) ginst->edge_count<< endl;
        if (notFound) {
            if (topDown && (double)frontierEdges > ((double)unexploredEdges / (double)a)) { topDown = !topDown;cout<<"switch"<<endl; }
            if(!topDown && ginst->vert_count/b>frontierSize) { topDown = !topDown; cout<<"switch back"<<endl;}
            dist++;
            nextDist++;
            frontierSize,frontierEdges= 0;
        }
    }
    cout << "End was " << dist << " nodes away from Start" << endl;

    return 0;
}

void clearDepth() {
#pragma omp parallel for
    for (int i = 0; i < ginst->vert_count; i++) {
        depth[i] = -1;
    }
}

void clearParents() {
#pragma omp parallel for
    for (int i = 0; i < ginst->vert_count; i++)
        parents[i] = -1;
}

void topDown_checkAndAdd(uint32_t i) { //TODO: eliminate the atomic __ functions to minimize thread-stalling.
    __sync_fetch_and_sub(&uncheckedVertices, 1);
    if (i == endNode) {
        notFound = false;
        return;
    }
    uint32_t beg_index = i;
    //get list of neighbors and add to the vector
    uint32_t end = ginst->beg_pos[beg_index + 1];

#pragma omp parallel for
    for (int j = ginst->beg_pos[beg_index]; j < end; j++) {
        __sync_fetch_and_sub(&unexploredEdges,1);
        uint32_t neighborNumber = ginst->csr[j];
        __sync_fetch_and_add(&frontierEdges, 1);
        if (depth[neighborNumber] == -1) {
            parents[neighborNumber] = i;
            depth[neighborNumber] = nextDist;
            __sync_fetch_and_add(&frontierSize, 1);
        }
    }
}

void bottomUp_checkAndAdd() {
#pragma omp parallel for
    for (uint32_t j = 0; j < ginst->vert_count; j++) { //for v in vertices
        if (notFound) { //essentially a break/stop. TODO: this is inefficient but necessary for the omp directive
            if (parents[j] == -1) { //if parents[v] is not found.
                int64_t beg_index = ginst->beg_pos[j];
                int64_t end_index = ginst->beg_pos[j + 1];
                bool stop = false;
#pragma omp parallel for
                for (uint32_t k = static_cast<uint32_t>(beg_index);
                     k < end_index; k++) { //looping through the neighbors of j
                    if (!stop) { //for the break TODO: this is inefficient but necessary for the omp directive
                        uint32_t neighborNumber = static_cast<uint32_t>(ginst->csr[k]); //n in neighbors[v]
                        if (depth[neighborNumber] ==
                            dist) { //if n is in the frontier, then n's depth is the current depth
                            if (neighborNumber == endNode) {
                                notFound = false;
                                stop = true;
                            }
                            parents[j] = neighborNumber;
                            depth[j] = nextDist;
                            stop = true;
                            __sync_fetch_and_add(&frontierSize, 1);
                            __sync_fetch_and_add(&frontierEdges,ginst->beg_pos[j+1]-ginst->beg_pos[j]);
                            __sync_fetch_and_sub(&unexploredEdges,ginst->beg_pos[j+1]-ginst->beg_pos[j]);
                        }
                    }
                }
            }
        }
    }
}