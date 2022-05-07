#ifndef SC_CLASSES_SIMULATOR_H
#define SC_CLASSES_SIMULATOR_H
#include "Network.h"

class Simulator
{
public:

    explicit Simulator(Network* network);
    void M1(int time);
    size_t transmistion_max_time = 10;

private:
    Network* network_ = nullptr;
    size_t clock_ = 0;
};

#endif //SC_CLASSES_SIMULATOR_H
