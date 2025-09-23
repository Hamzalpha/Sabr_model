#ifndef PRICER_HPP
#define PRICER_HPP
#include "../include/payoff.hpp"
double Montecarlo(payoff payoff,double spot, double rate, double vol, double expiry, double strike, unsigned number_paths);
double blkprice(double & F, double &K, double &T, double &sigma);

#endif // PRICER_HPP
