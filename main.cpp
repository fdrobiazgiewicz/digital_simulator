#include "Simulator.h"
#include  "Network.h"
#include <memory>
#include <iostream>
#include <tuple>

std::tuple<int, int, double, bool> menu(){
    int no_seed, sim_time;
    double lambda;
    std::cout<<"Enter the set of seeds <1,10>: ";
    std::cin >> no_seed;
    while (no_seed < 0 || no_seed > 10){
        std::cout<<"Value out of range.\nEnter the set of seeds <1,10>: ";
        std::cin >> no_seed;
    }
    std::cout<<"Enter time of simulation: ";
    std::cin>>sim_time;
    while (sim_time < 0){
        std::cout<<"Time of simulation has to be greater than 0!\nEnter time of simulations: ";
        std::cin>>sim_time;
    }
    std::cout<<"Enter lambda parameter: ";
    std::cin>>lambda;
    while (lambda < 0 && lambda > 10){
        std::cout<<"Lambda value out of range!\nEnter lambda parameter: ";
        std::cin>>lambda;
    }
    std::cout<<"Show debug messages? [y/n]\n";
    char decision;
    bool if_debug;
    std::cin>>decision;
    while (decision != 'y' && decision !='n') {
        std::cout << "Invalid input.\nShow debug messages? [y/n]\n";
        std::cin >> decision;
    }
    if (decision == 'y'){
        if_debug = true;
    }
    else if (decision == 'n'){
        if_debug = false;
    }
    return {no_seed, sim_time, lambda, if_debug};
}

int main(int argc, char* argv[])
{
    auto [no_seed, sim_time, lambda, if_debug] = menu();
    Network* network = new Network();
    Simulator simulator = Simulator(network);

    simulator.M1(sim_time, lambda, 0.1, 0.6, no_seed, if_debug);
}