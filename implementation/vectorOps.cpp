#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <cmath>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <functional>
#include <string>

std::vector<int> generateRandomVector(int size)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 30);

    std::vector<int> vec (size, 0);

    for (size_t i = 0; i < size; i++)
        vec[i] = dist(rng);

    return vec;
}


int constantFunc(std::vector<int> vec)
{
    return 1;
}

unsigned long long int vecSum(std::vector<int> vector)
{
    int sum_of_elems = std::accumulate(vector.begin(), vector.end(), 0);
    return sum_of_elems;
}

unsigned long long int vectorSum(std::vector<int> vec)
{
    int sum_of_elems = 0;
    for (auto& elem: vec)
        sum_of_elems += elem;
    return sum_of_elems;
}

unsigned long long int vectorProduction(std::vector<int> v)
{
    uint product = std::accumulate(v.begin(), v.end(), 1, std::multiplies<uint>());
    return product;
}

unsigned long long int vecProd(std::vector<int> vec)
{
    unsigned long long int elemProd = 1;
    for (auto& elem: vec)
        elemProd *= elem;
    return elemProd;
}

 double naivePolynomial(std::vector<int> vec, double x)
{
    double polynomial = 0;
    for (size_t i = 0; i < vec.size(); i++)
        polynomial += vec[i] * pow(x, i);
    return polynomial;
}

double horner(std::vector<int> v, double x)
{
     double polynomial = 0;
     for(std::vector<int>::const_reverse_iterator i = v.rbegin(); i != v.rend(); i++ )
         polynomial = polynomial * x + *i;
     return polynomial;
}

int main()
{
    int result = horner(vec, 1.5);
}
