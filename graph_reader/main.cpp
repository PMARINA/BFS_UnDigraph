#include <iostream>
#include "graph.h"
#include<vector>
#include <omp.h>

using namespace std;

uint32_t startNode = 1;
uint32_t endNode = 3072626;//last node in the graph
bool *checked;
bool notFound = true;
uint32_t arrayIndex = 0;
bool *current;
bool *nextRound;

void clearChecked();

void clearCurrent();

void clearNextRound();

void checkAndAdd(uint32_t i);

graph<long, long, int, long, long, char>
        *ginst;

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
    checked = new bool[ginst->vert_count];
    current = new bool[ginst->vert_count];
    nextRound = new bool[ginst->vert_count];
    clearChecked();
    uint32_t distance = 0;
    current[startNode] = true;
    checked[startNode] = true;
    omp_set_num_threads(8);
    cout << "Starting now" << endl;
    while (notFound) {
        //for should be in multithread
#pragma omp parallel for
        for (int i = 0; i < ginst->vert_count; i++) {
            if(current[i]){
                checkAndAdd(i);
            }
        }
        //should be in single thread
        cout << distance << endl;
        if (notFound) {
            distance++;
            swap(current, nextRound);
            clearNextRound();
        }
    }
    cout << "End was " << distance << " nodes away from Start" << endl;
    //**
    //You can implement your single threaded graph algorithm here.
    //like BFS, SSSP, PageRank and etc.

    //for(int i = 0; i < ginst->vert_count+1; i++)
    //{
    //    int beg = ginst->beg_pos[i];
    //    int end = ginst->beg_pos[i+1];
    //    std::cout<<i<<"'s neighor list: ";
    //    for(int j = beg; j < end; j++)
    //        std::cout<<ginst->csr[j]<<" ";
    //    std::cout<<"\n";
    //} 


    return 0;
}

void clearChecked() {
//#pragma omp parallel for
//    for(int i = 0; i<ginst->vert_count;i++){
//        checked[i]= false;
//    }

    memset(checked, 0,
           sizeof(*checked)); //I think this works, but if it breaks, the alternate code above should definitely work

}

void clearCurrent() {
    memset(current, 0,
           sizeof(*current));
}

void clearNextRound() {
    memset(nextRound, 0,
           sizeof(*nextRound));
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
        if (!checked[neighborNumber]) {
            checked[neighborNumber] = true;
            nextRound[neighborNumber] = true;
        }
    }
    return;
}