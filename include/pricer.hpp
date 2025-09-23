#ifndef PRICER_HPP
#define PRICER_HPP
#include "../include/payoff.hpp"
double Montecarlo(payoff payoff,double spot, double rate, double vol, double expiry, double strike, unsigned number_paths);

#endif // PRICER_HPP
