#ifndef PRICER_HPP
#define PRICER_HPP

double Montecarlo(double spot, double rate, double vol, double expiry, double strike, unsigned number_paths);

#endif // PRICER_HPP
