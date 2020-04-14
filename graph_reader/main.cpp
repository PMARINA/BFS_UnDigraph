#include <iostream>
#include "graph.h"
#include<vector>
#include <omp.h>

using namespace std;

uint32_t startNode = 1;
uint32_t endNode = 3072626;//last node in the graph
bool *checked;
bool notFound = true;

uint32_t *depth;

void clearDepth();

void checkAndAdd(uint32_t i);

graph<long, long, int, long, long, char>
        *ginst;
uint32_t dist = 0;
uint32_t nextDist = 1;

int main(int args, char **argv) {
    std::cout << "Input: ./exe beg csr weight\n";
    if (args != 4) {
        std::cout << "Wrong input\n";
        return -1;
    }

    const char *beg_file = argv[1];
    const char *csr_file = argv[2];
    const char *weight_file = argv[3];

    //template <file_vertex_t, file_index_t, file_weight_t
    //new_vertex_t, new_index_t, new_weight_t>

    ginst = new graph
            <long, long, int, long, long, char>
            (beg_file, csr_file, weight_file);
    depth = new uint32_t[ginst->vert_count];
    clearDepth();
    depth[startNode] = dist;
    omp_set_num_threads(8);
    cout << "Starting now" << endl;
    while (notFound) {
        //for should be in multithread
#pragma omp parallel for
        for (int i = 0; i < ginst->vert_count; i++) {
            if (depth[i] == dist) {
                checkAndAdd(i);
//                cout<<"Check and Add (" <<i << ");" <<endl;
            }
        }
        //should be in single thread
        cout << dist << endl;
        if (notFound) {
            dist++;
            nextDist++;
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


void checkAndAdd(uint32_t i) {
    uint32_t beg_index = i;
    if (i == endNode) {
        notFound = false;
        return;
    }
    //get list of neighbors and add to the vector
    uint32_t end;
    if (i != ginst->vert_count) {
        end = ginst->beg_pos[beg_index + 1]; //the starting place of the next node
    } else {
        end = ginst->vert_count - ginst->beg_pos[beg_index];
    }
#pragma omp parallel for
    for (int j = ginst->beg_pos[beg_index]; j < end; j++) {
        uint32_t neighborNumber = ginst->csr[j];
        if (depth[neighborNumber]==-1) {
            depth[neighborNumber] = nextDist;
        }
    }
    return;
}