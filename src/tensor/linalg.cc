#include "linalg.h"


namespace linalg {

/*
 * generate uniform vector in n-dim sphere, with sphere ratio = scale
 */
std::vector<float> gen_uniform_sp(uint32_t size, float scale){
    std::vector<float> x;
    for (uint32_t i = 0; i < size; ++i) {
        x.push_back((drand48() - 0.5) * 2.0);
    }

    l2_normalize(x);

    if (scale != 1.0) {
        x *= std::sqrt(scale);
    }

    return x;
}



void l2_normalize(std::vector<float> &x){
    std::vector<float> x2 = x * x;
    float sum = std::accumulate(x2.begin(), x2.end(), 0.0);
    x /= std::sqrt(sum);
}

}
