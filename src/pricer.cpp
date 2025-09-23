
#include "../include/pricer.hpp"
#include "../include/payoff.hpp"
#include <random>    // For std::mt19937, std::normal_distribution
#include <vector>    // For std::vector
#include <string>    // For std::string
#include <algorithm> // For std::max
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