#include <random>
#include <cmath>
#include <iostream>  // For std::cout, std::cin
#include <vector>    // For std::vector
#include <string>    // For std::string
#include <algorithm> // For std::max
double Montecarlo(const double& spot, const double& rate, const double& vol, const double& expiry, const double& strike, const unsigned& number_paths) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> normal(0,1);
    double sum {},payoff {};
    double S=spot;
    double discount = exp(-rate*expiry);
    for (unsigned j =0; j<number_paths; ++j){
        S=spot*exp((rate - 0.5*vol*vol)*expiry+vol*sqrt(expiry)*normal(gen));
        payoff = std::max(S - strike,0.0);
        sum += payoff;
    }
    double price = sum / number_paths;
    return discount*price;
}
int main()
{
    std::cout << "Call Option Price is:" <<std::endl;
    double spot = 100, rate= 0.01, vol=0.2, expiry=5, strike=100;
    unsigned number_paths = 500000;
    double price = Montecarlo(spot,rate,vol,expiry,strike,number_paths);
    std::cout<< price <<std::endl;
    return 0;
}