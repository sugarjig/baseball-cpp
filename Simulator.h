//
// Created by Samuel Jones on 5/12/26.
//

#ifndef BASEBALL_CPP_SIMULATOR_H
#define BASEBALL_CPP_SIMULATOR_H

#include <cstdio>

extern "C" {
#include "chadwick.h"
}

class Simulator {
private:
    CWGame* game;
    CWGameIterator* iter;
public:
    Simulator();

    ~Simulator();

    void SimulateGame();
};


#endif //BASEBALL_CPP_SIMULATOR_H
