# include "RandomGenerator.h"
#include <cmath>

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
