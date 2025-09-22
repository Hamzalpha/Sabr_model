#include <random>
#include <cmath>
#include <iostream>  // For std::cout, std::cin
#include "pricer.hpp"
int main()
{
    std::cout << "Call Option Price is:" <<std::endl;
    double spot = 100, rate= 0.01, vol=0.2, expiry=5, strike=100;
    unsigned number_paths = 500000;
    double price = Montecarlo(spot,rate,vol,expiry,strike,number_paths);
    std::cout<< price <<std::endl;
    return 0;
}