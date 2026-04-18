#ifndef PRICER_HPP
#define PRICER_HPP
#include <vector>
#include "../include/payoff.hpp"
double Montecarlo(payoff payoff,double spot, double rate, double vol, double expiry, double strike, unsigned number_paths);
double blkprice(double F, double K, double T, double sigma);
double SABR_implied_vol_ATM(double tho, double f, double alpha, double beta, double rho, double nu);
double SABR_implied_vol(double tho, double K, double f, double alpha, double beta, double rho, double nu);
double Forward_SABR(double K_r, double F, double tho, double alpha, double beta, double rho, double nu);
double SABR_Monte_Carlo(payoff payoff_, double K, double F, double year_frac, double alpha, double beta, double rho, double nu, int number_paths, int timestep);
double get_implied_vol(double price, double L, double K, double T);
void plot_sabr_implied_vols_beta(double F, double T, double alpha, double rho, double nu,
                                 double strike_min, double strike_max, int num_points);
double calculate_delta(double spot, double rate, double vol, double expiry, double strike, double h);
void plot_delta_vs_spot(double rate, double vol, double expiry, double strike, double spot_min, double spot_max, int num_points);
double calculate_vega(double spot, double rate, double vol, double expiry, double strike, double h);
void plot_vega_vs_spot(double rate, double vol, double expiry, double strike, double spot_min, double spot_max, int num_points);

// SABR Vega (Hagan approximation): sensitivity of option price to alpha
double black_vega(double F, double K, double T, double sigma);
double SABR_vega(double T, double K, double F, double alpha, double beta, double rho, double nu, double h = 1e-4);
void plot_SABR_vega_vs_strike(double F, double T, double alpha, double beta, double rho, double nu,
                               double strike_min, double strike_max, int num_points);

#endif
