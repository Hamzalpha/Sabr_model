#include <payoff.hpp>
#include <algorithm> 
payoff::payoff(OptionType type_, double strike_, double expiry_)
    : type(type_), strike(strike_), expiry(expiry_) {}

double payoff::operator()(const double spot) const {
    switch (type) {
        case call:
            return std::max(spot - strike, 0.0);
        case put:
            return std::max(strike - spot, 0.0);
        case forward:
            return spot - strike ;
        default:
            throw std::invalid_argument("Unknown option type");
    }
}