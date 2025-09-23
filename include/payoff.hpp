#ifndef PAYOFF_HPP
#define PAYOFF_HPP
enum OptionType { call, put, forward };

class payoff
{
private:
    OptionType type;
    double strike;
    double expiry;
public:
    payoff(OptionType type, double strike, double expiry);
    double operator()(const double strike) const;
};


#endif