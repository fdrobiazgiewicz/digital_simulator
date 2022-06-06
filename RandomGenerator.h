#ifndef DIGITAL_SIMULATOR_RANDOMGENERATOR_H
#define DIGITAL_SIMULATOR_RANDOMGENERATOR_H


class RandomGenerator
{
public:
    RandomGenerator(int kernel);
    virtual ~RandomGenerator();

    double Rand();
    int Rand(int min, int max);
    int RndExp(double lambda);
    int RndZeroOne(double p);

    int get_kernel() { return kernel_; }

private:
    int kernel_;
    const double kM = 2147483647.0;
    static const int kA = 16807;
    static const int kQ = 127773;
    static const int kR = 2836;
};

#endif //DIGITAL_SIMULATOR_RANDOMGENERATOR_H

