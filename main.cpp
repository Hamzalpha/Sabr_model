#include "matplotlibcpp.h"
#include <vector>
#include <random>
#include <cmath>
#include <iostream>  
#include "pricer.hpp"
#include "payoff.hpp"
namespace plt = matplotlibcpp;

int main()
{
        double spot = 100, rate = 0.0, vol = 0.2, expiry = 5, strike = 100;
        unsigned number_paths = 500000;
        payoff CallOption(call, strike, expiry);
        payoff PutOption(put, strike, expiry);
        payoff ForwardOption(forward, strike, expiry);
        double price_call = Montecarlo(CallOption, spot, rate, vol, expiry, strike, number_paths);
        double price_put = Montecarlo(PutOption, spot, rate, vol, expiry, strike, number_paths);
        double price_forward = Montecarlo(ForwardOption, spot, rate, vol, expiry, strike, number_paths);
        double blk_call = blkprice(spot, strike, expiry, vol);
        std::cout << "Call Option Price is:" << std::endl;
        std::cout << price_call << std::endl;
        std::cout << "Put Option Price is:" << std::endl;
        std::cout << price_put << std::endl;
        std::cout << "Forward Contract Price is:" << std::endl;
        std::cout << price_forward << std::endl;
        std::cout << "Black-Scholes Call Price is:" << std::endl;
        std::cout << blk_call << std::endl;
        double alpha = 0.2, beta = 0.5, rho = 0.0, nu = 0.3;
        int timestep = 50;
        int number_sabr_paths = 500000;
        double test_price = blk_call;
        plot_sabr_implied_vols_beta(100, 1.0, 0.2, 0.0, 0.3, 80, 120, 50);
        plot_delta_vs_spot(rate, vol, expiry, strike, 50, 150, 100);
    return 0;

   
}