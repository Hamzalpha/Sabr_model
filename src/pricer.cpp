
#include "../include/pricer.hpp"
#include "../include/payoff.hpp"
#include "../include/maths.hpp"
#include <random>    // For std::mt19937, std::normal_distribution
#include <vector>    // For std::vector
#include <string>    // For std::string
#include <algorithm> // For std::max
using namespace maths; // Brings all names from maths into scope

double Montecarlo(payoff payoff_,double spot, double rate, double vol, double expiry, double strike, unsigned number_paths) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> normal(0,1);
    double sum {},payoff {};
    double S=spot;
    double discount = exp(-rate*expiry);
    for (unsigned j =0; j<number_paths; ++j){
        S=spot*exp((rate - 0.5*vol*vol)*expiry+vol*sqrt(expiry)*normal(gen));
        sum += payoff_(S);
    }
    double price = sum / number_paths;
    return discount*price;
}
double blkprice(double & F, double &K, double &T, double &sigma) {
    // Black pricer (undiscounted)
    double d1 = (std::log(F / K) + 0.5 * sigma * sigma * T) / (sigma * std::sqrt(T));
    double d2 = (std::log(F / K) - 0.5 * sigma * sigma * T) / (sigma * std::sqrt(T));
    double black_price = F * norm_cdf(d1) - K * norm_cdf(d2);
    return black_price;
}
