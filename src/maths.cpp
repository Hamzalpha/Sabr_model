#include "maths.hpp"
#include <algorithm>
#include <cmath>
namespace maths {
    double norm_cdf(double x) {
        return 0.5 * std::erfc(-x / std::sqrt(2));
    }

    double norm_pdf(double x) {
        return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
    }
}
