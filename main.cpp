#include "Simulator.h"
#include  "Network.h"
#include <memory>
#include <iostream>

void menu(){
    int no_sim, sim_time;
    double lambda;
    std::cout<<"Enter number of simulations: ";
    std::cin>>no_sim;
    while (no_sim < 0){
        std::cout<<"Number of simulation has to be greater than 0!\nEnter number of simulations: ";
        std::cin>>no_sim;
    }
    std::cout<<"Enter time of simulation: ";
    std::cin>>sim_time;
    while (sim_time < 0){
        std::cout<<"Time of simulation has to be greater than 0!\nEnter time of simulations: ";
        std::cin>>sim_time;
    }
    std::cout<<"Enter lamda parameter: ";
    std::cin>>lambda;
    while (lambda < 0 && lambda > 10){
        std::cout<<"Lambda value out of range!\nEnter lambda parameter: ";
        std::cin>>lambda;
    }
}

int main(int argc, char* argv[])
{
//    menu();
    Network* network = new Network();
    Simulator simulator = Simulator(network);

    simulator.M1(400, 0.3, 1, 0.6);
}