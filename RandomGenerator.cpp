# include "RandomGenerator.h"
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <iterator>

RandomGenerator::RandomGenerator(int kernel): kernel_(kernel)
{

}

RandomGenerator::~RandomGenerator()
{

}

double RandomGenerator::Rand()
{
    int h = kernel_ / kQ;
    kernel_ = kA * (kernel_ - kQ * h) - kR * h;
    if (kernel_ < 0)
        kernel_ = kernel_ + static_cast<int>(kM);
    return kernel_ / kM;
}

int RandomGenerator::Rand(int min, int max)
{
    return Rand() * (max - min) + min;
}

int RandomGenerator::RndExp(double lambda)
{
    double k = Rand();
    return -(1.0 / lambda) * log(k);
}

int RandomGenerator::RndZeroOne(double p)
{
    double k = Rand();
    if (k < p)
        return 1;
    return 0;
}

void RandomGenerator::GenerateSeeds(int no_seeds){
    auto uniform_generator = RandomGenerator(44);
    std::vector<int> seeds;
    for (int i = 0; i < no_seeds; ++i) {
        uniform_generator.Rand();
        std::cout << "seed: " << uniform_generator.get_kernel() << std::endl;
        seeds.push_back(uniform_generator.get_kernel());
    }
    // Save data
    std::ofstream output_file("./seeds.txt");
    std::ostream_iterator<int> output_iterator(output_file, "\n");
    std::copy(seeds.begin(), seeds.end(), output_iterator);
}

