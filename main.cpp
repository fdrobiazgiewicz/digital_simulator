#include "Simulator.h"
#include  "Network.h"
#include <memory>

int main(int argc, char* argv[])
{
    Network* network = new Network();
    Simulator simulator = Simulator(network);

    simulator.M1(400);


}