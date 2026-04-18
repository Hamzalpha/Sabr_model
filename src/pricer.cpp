#include "../include/pricer.hpp"
#include "../include/payoff.hpp"
#include "../include/maths.hpp"
#include "../include/matplotlibcpp.h"
#include <random>    
#include <vector>    
#include <string>    
#include <algorithm> 
#include <cmath>
namespace plt = matplotlibcpp;
using namespace maths; 
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
double blkprice(double F, double K, double T, double sigma) {
    double d1 = (std::log(F / K) + 0.5 * sigma * sigma * T) / (sigma * std::sqrt(T));
    double d2 = (std::log(F / K) - 0.5 * sigma * sigma * T) / (sigma * std::sqrt(T));
    double black_price = F * norm_cdf(d1) - K * norm_cdf(d2);
    return black_price;
}


double SABR_implied_vol_ATM(double tho, double f, double alpha, double beta, double rho, double nu) {
    double y = alpha * (1 +
        (std::pow(1 - beta, 2) / 24.0) * (std::pow(alpha, 2) / std::pow(f, 2 - 2 * beta))
        + (rho * beta * nu * alpha) / (4.0 * std::pow(f, 1 - beta))
        + ((2 - 3 * std::pow(rho, 2)) / 24.0) * std::pow(nu, 2)
    ) * tho;

    double x = std::pow(f, 1 - beta);
    double sigma_implied = y / x;
    return sigma_implied;
}


double SABR_implied_vol(double tho, double K, double f, double alpha, double beta, double rho, double nu) {
    if (std::abs(K - f) < 1e-8) {
        return SABR_implied_vol_ATM(tho, f, alpha, beta, rho, nu);
    }
    double y = alpha * (1 +
        (std::pow(1 - beta, 2) / 24.0) * (std::pow(alpha, 2) / std::pow(f, 2 - 2 * beta))
        + (rho * beta * nu * alpha) / (4.0 * std::pow(f, 1 - beta))
        + ((2 - 3 * std::pow(rho, 2)) / 24.0) * std::pow(nu, 2)
    ) * tho;

    if (K <= 0 || f <= 0 || alpha <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    double log_fk = std::log(f / K);
    double fk_pow = std::pow(f * K, 0.5 * (1 - beta));
    double x = fk_pow *
        (1 + (std::pow(1 - beta, 2) / 24.0) * std::pow(log_fk, 2)
           + (std::pow(1 - beta, 4) / 1920.0) * std::pow(log_fk, 4));
    double z = (nu / alpha) * fk_pow * log_fk;
    double denom = (1 - rho);
    double sqrt_arg = 1 - 2 * rho * z + z * z;
    if (denom == 0 || sqrt_arg < 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    double xi = std::log((std::sqrt(sqrt_arg) + z - rho) / denom);
    if (xi == 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    double result = (y / x) * (z / xi);
    if (!std::isfinite(result) || result < 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return result;
}

double Forward_SABR(double K_r, double F, double tho, double alpha, double beta, double rho, double nu) {
    if (F == K_r) {
        return SABR_implied_vol_ATM(tho, F, alpha, beta, rho, nu);
    } else {
        return SABR_implied_vol(tho, K_r, F, alpha, beta, rho, nu);
    }
}


double get_implied_vol(double price, double L, double K, double T) {
    double tol = 1e-8;
    int max_iter = 100;
    double vol = 0.2; 

    for (int i = 0; i < max_iter; ++i) {
        double diff = price - blkprice(L, K, T, vol);
        double d_vol = 1e-5;
        double vega = (blkprice(L, K, T, vol + d_vol) - blkprice(L, K, T, vol - d_vol)) / (2 * d_vol);

        if (std::abs(diff) < tol) {
            return vol;
        }
        if (vega == 0) {
            throw std::runtime_error("Vega is zero. Stop");
        }
        vol += diff / vega;
        if (vol < 0) vol = 1e-6; 
    }
    throw std::runtime_error("Implied volatility not found.");
}


double SABR_Monte_Carlo(payoff payoff_,double K, double F, double year_frac, double alpha, double beta, double rho, double nu, int number_paths, int timestep) {
    std::vector<double> forwards(number_paths, F);
    std::vector<double> volatilities(number_paths, alpha);
    double dt = year_frac / timestep;
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<double> W1(number_paths), W2(number_paths), W3(number_paths);
    std::normal_distribution<double> standard_normal(0.0, 1.0);
    for (size_t i = 0; i < number_paths; ++i) {
        for (size_t j = 0; j < timestep; ++j) {
            W1[i] = standard_normal(gen);
            W2[i] = standard_normal(gen);
            W3[i] = rho * W1[i] + sqrt(1 - rho * rho) * W2[i];
            forwards[i] += std::sqrt(dt) * volatilities[i] * std::pow(forwards[i], beta) * W1[i];
            volatilities[i] *= std::exp((-0.5 * nu * nu) * dt + nu * std::sqrt(dt) * W3[i]);
        }
    }
    std::vector<double> DiscPayoff(number_paths);
    for (int j = 0; j < number_paths; ++j) {
        DiscPayoff[j] = payoff_(forwards[j]);
    }

    double price = std::accumulate(DiscPayoff.begin(), DiscPayoff.end(), 0.0) / number_paths;
    try {
        return get_implied_vol(price, F, K, year_frac);
    } catch (const std::exception& e) {
        std::cerr << "SABR_Monte_Carlo: Implied volatility not found for K=" << K << ", F=" << F << ": " << e.what() << std::endl;
        return std::numeric_limits<double>::quiet_NaN();
    }
    
    }

double calculate_vega(double spot, double rate, double vol, double expiry, double strike, double h = 1e-4) {
    double price_up = blkprice(spot, strike, expiry, vol + h);
    double price_down = blkprice(spot, strike, expiry, vol - h);
    return (price_up - price_down) / (2 * h);
}
double calculate_delta(double spot, double rate, double vol, double expiry, double strike, double h = 1e-4) {
    double price_up = blkprice(spot + h, strike, expiry, vol);
    double price_down = blkprice(spot - h, strike, expiry, vol);
    return (price_up - price_down) / (2 * h);
}

void plot_vega_vs_spot(double rate, double vol, double expiry, double strike, double spot_min, double spot_max, int num_points) {
    std::vector<double> spots(num_points);
    std::vector<double> vegas(num_points);
    double dspot = (spot_max - spot_min) / (num_points - 1);
    for (int j=0;j<num_points;++j){
        spots[j] = spot_min + j * dspot;
        vegas[j] = calculate_vega(spots[j], rate, vol, expiry, strike);
    }
    plt::figure();
    plt::plot(spots, vegas);
    plt::xlabel("Spot");
    plt::ylabel("Vega");
    plt::title("Vega vs Spot");
    plt::grid(true);
    plt::save("results/vega_vs_spot.png");
    plt::show();
}

void plot_delta_vs_spot(double rate, double vol, double expiry, double strike, double spot_min, double spot_max, int num_points) {
    std::vector<double> spots(num_points);
    std::vector<double> deltas(num_points);
    for (int i = 0; i < num_points; ++i) {
        spots[i] = spot_min + i * (spot_max - spot_min) / (num_points - 1);
        deltas[i] = calculate_delta(spots[i], rate, vol, expiry, strike);
    }
    plt::figure();
    plt::plot(spots, deltas);
    plt::xlabel("Spot");
    plt::ylabel("Delta");
    plt::title("Delta vs Spot");
    plt::grid(true);
    plt::save("results/delta_vs_spot.png");
    plt::show();
}



void plot_sabr_implied_vols_beta(double F, double T, double alpha, double rho, double nu,
                                 double strike_min, double strike_max, int num_points) {
    std::vector<double> betas = {0.1, 0.5, 0.9};
    std::vector<double> strikes(num_points);
    for (int i = 0; i < num_points; ++i) {
        strikes[i] = strike_min + i * (strike_max - strike_min) / (num_points - 1);
    }
    plt::figure();
    for (double beta : betas) {
        std::vector<double> vols(num_points);
        std::cout << "Beta: " << beta << " Vols: ";
        for (int i = 0; i < num_points; ++i) {
            try {
                double v = SABR_implied_vol(T, strikes[i], F, alpha, beta, rho, nu);
                if (v > 0 && v < 10)
                    vols[i] = v;
                else
                    vols[i] = std::numeric_limits<double>::quiet_NaN();
            } catch (const std::exception& e) {
                vols[i] = std::numeric_limits<double>::quiet_NaN();
                std::cerr << "Exception for beta=" << beta << ", strike=" << strikes[i] << ": " << e.what() << std::endl;
            }
            if (i < 5) std::cout << vols[i] << " ";
        }
        std::cout << std::endl;
        plt::plot(strikes, vols, { {"label", "beta=" + std::to_string(beta)} });
    }
    plt::legend();
    plt::xlabel("Strike");
    plt::ylabel("Implied Volatility");
    plt::title("SABR Hagan Implied Vol vs Strike (varying beta)");
    plt::save("results/sabr_beta.png");
    plt::show();
}

// Analytical Black-76 vega: dV/d_sigma
double black_vega(double F, double K, double T, double sigma) {
    double d1 = (std::log(F / K) + 0.5 * sigma * sigma * T) / (sigma * std::sqrt(T));
    return F * std::sqrt(T) * norm_pdf(d1);
}

// SABR Vega using Hagan approximation: dV/d_alpha
// Chain rule: dV/d_alpha = (dV/d_sigma) * (d_sigma_SABR/d_alpha)
// - dV/d_sigma computed analytically via black_vega
// - d_sigma_SABR/d_alpha computed via central finite difference on Hagan formula
double SABR_vega(double T, double K, double F, double alpha, double beta, double rho, double nu, double h) {
    double sigma = Forward_SABR(K, F, T, alpha, beta, rho, nu);
    if (!std::isfinite(sigma) || sigma <= 0)
        return std::numeric_limits<double>::quiet_NaN();

    double bvega = black_vega(F, K, T, sigma);

    double sigma_up   = Forward_SABR(K, F, T, alpha + h, beta, rho, nu);
    double sigma_down = Forward_SABR(K, F, T, alpha - h, beta, rho, nu);
    if (!std::isfinite(sigma_up) || !std::isfinite(sigma_down))
        return std::numeric_limits<double>::quiet_NaN();

    double dsigma_dalpha = (sigma_up - sigma_down) / (2.0 * h);
    return bvega * dsigma_dalpha;
}

void plot_SABR_vega_vs_strike(double F, double T, double alpha, double beta, double rho, double nu,
                               double strike_min, double strike_max, int num_points) {
    std::vector<double> strikes(num_points), vegas(num_points);
    for (int i = 0; i < num_points; ++i) {
        strikes[i] = strike_min + i * (strike_max - strike_min) / (num_points - 1);
        vegas[i] = SABR_vega(T, strikes[i], F, alpha, beta, rho, nu);
    }
    plt::figure();
    plt::plot(strikes, vegas);
    plt::xlabel("Strike");
    plt::ylabel("SABR Vega (dV/dAlpha)");
    plt::title("SABR Vega vs Strike (Hagan Approximation)");
    plt::grid(true);
    plt::save("results/sabr_vega_vs_strike.png");
    plt::show();
}
