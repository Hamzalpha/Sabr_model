#include "maths.hpp"
#include <algorithm> 
#include <cmath>     
namespace maths {
    // Standard normal cumulative distribution function
    double norm_cdf(double x) {
        return 0.5 * std::erfc(-x / std::sqrt(2));
    }


}